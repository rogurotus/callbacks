
fn main() {
    cxx_build::bridge("src/main.rs")
        .file("src/cb.cc")
        .file("src/cb2.cc")
        .flag_if_supported("-std=c++14")
        .compile("callbacks");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/blobstore.cc");
    println!("cargo:rerun-if-changed=include/blobstore.h");
}