use std::path::Path;
use std::env;

fn main() {
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    
    
    match env::consts::OS {
        "windows" => {
            println!("cargo:rustc-link-search=native={}", Path::new(&dir).join("lib").join("windows").display());
            println!("cargo:rustc-link-lib=user32");
            println!("cargo:rustc-link-lib=kernel32");
            println!("cargo:rustc-link-lib=gdi32");
            println!("cargo:rustc-link-lib=shell32");
        },
        "linux" => {
            println!("cargo:rustc-link-search=native={}", Path::new(&dir).join("lib").join("linux").display());
        },
        _ => {
            panic!("Unsupported OS")
        }
    }

    println!("cargo:rustc-link-lib=static=cgl");    
    println!("cargo:rustc-link-lib=static=glfw3");
    println!("cargo:rustc-link-lib=static=glad");
    
}