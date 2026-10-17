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


def available_examples() -> list[str]:
    return [match.group(1) for match in EXAMPLE_PATTERN.finditer(EXAMPLES_CMAKE.read_text(encoding="utf-8"))]


def example_requirements(example: str) -> set[str]:
    for match in EXAMPLE_PATTERN.finditer(EXAMPLES_CMAKE.read_text(encoding="utf-8")):
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
        "SDK first, or pass --toolchain-file explicitly."
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build one C example using this repo's CMake setup."
    )
    parser.add_argument("--example", help="CMake target name; use --list to see choices")
    parser.add_argument(
        "--list", action="store_true", help="list available C examples and exit"
    )
    parser.add_argument(
        "--platform",
        default="native",
        help=(
            "output/build label (default: native); 'wasm' or 'web' selects "
            "Emscripten, other labels use the host compiler unless a toolchain is supplied"
        ),
    )
    parser.add_argument(
        "--toolchain-file",
        type=Path,
        help="CMake toolchain file for a cross-platform build",
    )
    parser.add_argument(
        "--no-auto-fetch",
        action="store_true",
        help="do not fetch FreeType automatically for examples that require it",
    )
    parser.add_argument("--generator", help="CMake generator, e.g. Ninja or Ninja Multi-Config")
    parser.add_argument("--config", default="Release", help="build configuration (default: Release)")
    parser.add_argument("--jobs", type=int, help="number of parallel build jobs")
    parser.add_argument("--build-dir", type=Path, help="override the per-platform CMake build directory")
    parser.add_argument(
        "--output-dir", type=Path, help="override where the built executable/assets are written"
    )
    parser.add_argument(
        "--cmake-arg",
        action="append",
        default=[],
        help="extra CMake configure argument; may be repeated",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    examples = available_examples()

    if args.list:
        print("\n".join(examples))
        return 0

    if not args.example:
        print("error: --example is required (or use --list)", file=sys.stderr)
        return 2
    if args.example not in examples:
        print(f"error: unknown example {args.example!r}; use --list", file=sys.stderr)
        return 2
    if not PLATFORM_PATTERN.fullmatch(args.platform) or args.platform in {".", ".."}:
        print("error: --platform must be a simple name without path separators", file=sys.stderr)
        return 2
    if args.jobs is not None and args.jobs < 1:
        print("error: --jobs must be at least 1", file=sys.stderr)
        return 2

    platform = args.platform.lower()
    is_wasm = platform in {"wasm", "web"}
    try:
        toolchain_file = args.toolchain_file.resolve() if args.toolchain_file else None
        if is_wasm and toolchain_file is None:
            toolchain_file = emscripten_toolchain()
        if toolchain_file is not None and not toolchain_file.is_file():
            raise RuntimeError(f"CMake toolchain file not found: {toolchain_file}")
    except RuntimeError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

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

    build_dir.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)

    cmake = shutil.which("cmake")
    if not cmake:
        print("error: cmake was not found on PATH", file=sys.stderr)
        return 2

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
    if "REQUIRES_FREETYPE" in example_requirements(args.example) and not args.no_auto_fetch:
        configure.append("-DCGL_FETCH_MISSING_FREETYPE=ON")
    if is_wasm:
        configure.append("-DCGL_EXAMPLE_OUTPUT_NAME=cglweb")
    if toolchain_file:
        configure.append(f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}")
    if args.generator:
        configure.extend(["-G", args.generator])
    configure.extend(args.cmake_arg)

    print(f"Building {args.example} for {args.platform}", flush=True)
    print(f"Build files: {build_dir}", flush=True)
    print(f"Output:      {output_dir}", flush=True)
    result = subprocess.run(configure, cwd=ROOT, check=False)
    if result.returncode:
        return result.returncode

    build = [cmake, "--build", str(build_dir), "--target", args.example, "--config", args.config]
    if args.jobs:
        build.extend(["--parallel", str(args.jobs)])
    result = subprocess.run(build, cwd=ROOT, check=False)
    if result.returncode:
        return result.returncode

    if is_wasm:
        print(f"Website build ready at: {output_dir}", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
