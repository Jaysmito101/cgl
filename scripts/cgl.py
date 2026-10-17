#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EXAMPLES_CMAKE = ROOT / "examples" / "c" / "CMakeLists.txt"
EXAMPLE_PATTERN = re.compile(
    r"^\s*cgl_add_example\(\s*([A-Za-z0-9_.-]+)\s+([^\s)]+)([^)]*)\)",
    re.MULTILINE,
)
PLATFORM_PATTERN = re.compile(r"[A-Za-z0-9][A-Za-z0-9_.-]*\Z")


def example_declarations() -> list[re.Match[str]]:
    source = EXAMPLES_CMAKE.read_text(encoding="utf-8")
    return list(EXAMPLE_PATTERN.finditer(source))


def available_examples() -> list[str]:
    return [match.group(1) for match in example_declarations()]


def example_requirements(example: str) -> set[str]:
    for match in example_declarations():
        if match.group(1) == example:
            return set(match.group(3).split())
    return set()


def emscripten_toolchain() -> Path:
    candidates: list[Path] = []
    emsdk = os.environ.get("EMSDK")
    if emsdk:
        candidates.append(
            Path(emsdk)
            / "upstream"
            / "emscripten"
            / "cmake"
            / "Modules"
            / "Platform"
            / "Emscripten.cmake"
        )

    emcc = shutil.which("emcc")
    if emcc:
        candidates.append(
            Path(emcc).resolve().parent
            / "cmake"
            / "Modules"
            / "Platform"
            / "Emscripten.cmake"
        )

    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()

    raise RuntimeError(
        "Could not find the Emscripten CMake toolchain. Activate the Emscripten "
        "SDK first, or pass --toolchain explicitly."
    )


def add_build_options(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--platform",
        default="native",
        help="build label (default: native); use wasm for Emscripten",
    )
    parser.add_argument(
        "--toolchain",
        type=Path,
        help="CMake toolchain file for cross-compilation",
    )
    parser.add_argument("--generator", help="CMake generator, e.g. Ninja")
    parser.add_argument("--config", default="Release", help="build configuration (default: Release)")
    parser.add_argument("--jobs", type=int, help="number of parallel build jobs")
    parser.add_argument("--build-dir", type=Path, help="override the CMake build directory")
    parser.add_argument("--output-dir", type=Path, help="override the executable/assets directory")
    parser.add_argument(
        "--cmake-arg",
        action="append",
        default=[],
        help="extra CMake configure argument; may be repeated",
    )
    parser.add_argument(
        "--no-fetch",
        action="store_true",
        help="do not automatically fetch FreeType when an example requires it",
    )


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    arguments = list(sys.argv[1:] if argv is None else argv)
    program_args: list[str] = []

    if arguments and arguments[0] == "run" and "--" in arguments:
        separator = arguments.index("--")
        program_args = arguments[separator + 1 :]
        arguments = arguments[:separator]

    parser = argparse.ArgumentParser(
        prog="cgl",
        description="List, build, and run CGL C examples.",
        epilog=(
            "Examples:\n"
            "  python scripts/cgl.py list\n"
            "  python scripts/cgl.py build hello_cube\n"
            "  python scripts/cgl.py run hello_cube"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    commands = parser.add_subparsers(dest="command", required=True)

    list_parser = commands.add_parser("list", help="list available C examples")
    list_parser.add_argument("filter", nargs="?", help="show names containing this text")

    build_parser = commands.add_parser("build", help="build one C example")
    build_parser.add_argument("example", help="example name (see `cgl list`)")
    add_build_options(build_parser)

    run_parser = commands.add_parser(
        "run",
        help="build and run a native C example",
        epilog="Pass arguments to the example after `--`, e.g. `run hello_cube -- --help`.",
    )
    run_parser.add_argument("example", help="example name (see `cgl list`)")
    add_build_options(run_parser)

    parsed = parser.parse_args(arguments)
    if parsed.command == "run":
        parsed.program_args = program_args
    elif program_args:
        parser.error("program arguments can only be passed to the run command")
    return parsed


def validate_build_args(args: argparse.Namespace, examples: list[str]) -> tuple[str, Path | None] | None:
    if args.example not in examples:
        print(f"error: unknown example {args.example!r}; use `cgl list` to see choices", file=sys.stderr)
        return None
    if not PLATFORM_PATTERN.fullmatch(args.platform) or args.platform in {".", ".."}:
        print("error: --platform must be a simple name without path separators", file=sys.stderr)
        return None
    if args.jobs is not None and args.jobs < 1:
        print("error: --jobs must be at least 1", file=sys.stderr)
        return None

    platform = args.platform.lower()
    is_wasm = platform in {"wasm", "web"}
    try:
        toolchain_file = args.toolchain.resolve() if args.toolchain else None
        if is_wasm and toolchain_file is None:
            toolchain_file = emscripten_toolchain()
        if toolchain_file is not None and not toolchain_file.is_file():
            raise RuntimeError(f"CMake toolchain file not found: {toolchain_file}")
    except RuntimeError as error:
        print(f"error: {error}", file=sys.stderr)
        return None

    return platform, toolchain_file


def build_example(args: argparse.Namespace) -> tuple[int, Path | None]:
    examples = available_examples()
    if args.example not in examples:
        print(f"error: unknown example {args.example!r}; use `cgl list` to see choices", file=sys.stderr)
        return 2, None

    validated = validate_build_args(args, examples)
    if validated is None:
        return 2, None
    platform, toolchain_file = validated
    is_wasm = platform in {"wasm", "web"}

    build_dir = (
        args.build_dir.resolve()
        if args.build_dir
        else ROOT / "build" / "examples" / ".cmake" / args.platform
    )
    if args.output_dir:
        output_dir = args.output_dir.resolve()
    elif is_wasm:
        output_dir = ROOT / "site" / "builds" / args.example
    else:
        output_dir = ROOT / "build" / "examples" / args.platform / args.example

    cmake = shutil.which("cmake")
    if not cmake:
        print("error: CMake was not found on PATH", file=sys.stderr)
        return 2, None

    build_dir.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)

    configure = [
        cmake,
        "-S",
        str(ROOT),
        "-B",
        str(build_dir),
        f"-DCGL_BUILD_EXAMPLE={args.example}",
        f"-DCGL_EXAMPLE_OUTPUT_DIRECTORY={output_dir}",
        f"-DCMAKE_BUILD_TYPE={args.config}",
    ]
    if "REQUIRES_FREETYPE" in example_requirements(args.example) and not args.no_fetch:
        configure.append("-DCGL_FETCH_MISSING_FREETYPE=ON")
    if is_wasm:
        configure.append("-DCGL_EXAMPLE_OUTPUT_NAME=cglweb")
    if toolchain_file:
        configure.append(f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}")
    if args.generator:
        configure.extend(["-G", args.generator])
    configure.extend(args.cmake_arg)

    print(f"Building {args.example} ({args.platform}, {args.config})", flush=True)
    print(f"Build directory: {build_dir}", flush=True)
    print(f"Output directory: {output_dir}", flush=True)
    result = subprocess.run(configure, cwd=ROOT, check=False)
    if result.returncode:
        return result.returncode, None

    build = [cmake, "--build", str(build_dir), "--target", args.example, "--config", args.config]
    if args.jobs:
        build.extend(["--parallel", str(args.jobs)])
    result = subprocess.run(build, cwd=ROOT, check=False)
    if result.returncode:
        return result.returncode, None

    if is_wasm:
        print(f"Web build ready: {output_dir}", flush=True)
        return 0, None

    executable = output_dir / f"{args.example}{'.exe' if os.name == 'nt' else ''}"
    if executable.is_file():
        print(f"Build ready: {executable}", flush=True)
        return 0, executable

    candidates = list(output_dir.rglob(f"{args.example}{'.exe' if os.name == 'nt' else ''}"))
    if candidates:
        executable = candidates[0]
        print(f"Build ready: {executable}", flush=True)
        return 0, executable

    print(f"error: build succeeded but executable was not found under {output_dir}", file=sys.stderr)
    return 1, None


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    if args.command == "list":
        examples = available_examples()
        if args.filter:
            examples = [name for name in examples if args.filter.casefold() in name.casefold()]
        if examples:
            print("\n".join(examples))
        else:
            print("No matching examples.")
        return 0

    if args.command == "run":
        if args.platform.lower() != "native" or args.toolchain:
            print("error: `run` supports native host builds only", file=sys.stderr)
            return 2

    result, executable = build_example(args)
    if result or args.command == "build":
        return result
    if executable is None:
        print("error: this build does not produce a native executable", file=sys.stderr)
        return 2

    print(f"Running: {executable}\n", flush=True)
    return subprocess.run([str(executable), *args.program_args], cwd=ROOT, check=False).returncode


if __name__ == "__main__":
    raise SystemExit(main())
