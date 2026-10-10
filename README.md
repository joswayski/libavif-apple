# libavif-apple

A static `libavif.xcframework` with the [libavif](https://github.com/AOMediaCodec/libavif)
AVIF encoder and [aom](https://aomedia.googlesource.com/aom/)'s AV1 encoder,
for macOS and iOS apps that need to write AVIF images. Built for photo
attachments in [Caper](https://caper.chat), a place for your people to chat
about anything ([source](https://github.com/joswayski/caper)).

| | |
| --- | --- |
| libavif | 1.4.2 |
| aom | 3.15.1 (signed release tarball) |
| Slices | macOS arm64 + x86_64 (14.0), iOS arm64 (17.0), iOS simulator arm64 + x86_64 (17.0) |
| Contents | Encoder only, 8-bit only. No AV1 decoder, high bit depth, libyuv, libsharpyuv or libvmaf. |
| CPU code | arm64: Neon, with dot-product/i8mm paths chosen at run time (no SVE; no Apple CPU has it). x86_64: SSE/AVX via nasm. |

Both sources are pinned by SHA-256 in `build.sh`. Swift imports it as
`import libavif` (module map in `Headers/libavif/`).

## Releases

Every push to `main` builds the XCFramework on a GitHub macOS runner, links a
small encoder smoke test (`test/smoke.c`) against every slice, runs it on
arm64 and x86_64 macOS, checks that Swift can import the module, and publishes
`libavif.xcframework.zip` as a release named
`libavif-<version>-aom-<version>-r<RELEASE_REVISION>`. An existing release is
never replaced: consumers pin the zip's SHA-256 (in the release notes and the
`.sha256` asset), so a rebuild of the same versions bumps `RELEASE_REVISION`.

To upgrade, change the versions and SHA-256s in `build.sh`, update the texts in
`licenses/` (the build fails if they no longer contain the upstream files) and
push.

## Using it

Download the zip, verify its SHA-256, unzip, and link `libavif.xcframework`.
Ship the licence texts in the zip with your app. With Swift Package Manager:

```swift
.binaryTarget(
    name: "libavif",
    url: "https://github.com/joswayski/libavif-apple/releases/download/<tag>/libavif.xcframework.zip",
    checksum: "<sha256>"
)
```

Quality note: libavif 1.3+ with aom 3.13+ encodes still images with aom's
`tune=iq`, whose quality scale differs from earlier versions. Caper sets
`c:tune=ssim` to keep the libavif 1.0 quality scale.

## Building locally

Requires Xcode, CMake and (for x86_64 SIMD) nasm: `brew install cmake nasm ninja`,
then `./build.sh`. Output: `dist/libavif.xcframework`.

## Licences

libavif: BSD 2-Clause (`licenses/libavif-LICENSE.txt`, including its bundled
code). aom: BSD 2-Clause plus the Alliance for Open Media Patent License 1.0
(`licenses/libaom-LICENSE.txt`, `licenses/libaom-PATENTS.txt`, including the
third-party code compiled in).
