// Encodes a gradient the way Caper's Apple client does (8-bit 4:2:0, libavif
// quality, speed 6, tune=ssim) and checks the result is an AVIF file.
#include <avif/avif.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("%s\n", avifVersion());
    char codecs[256];
    avifCodecVersions(codecs);
    printf("%s\n", codecs);
    if (!strstr(codecs, "aom [enc")) return fprintf(stderr, "no aom encoder\n"), 1;

    const uint32_t size = 256;
    avifRGBImage rgb;
    avifImage *image = avifImageCreate(size, size, 8, AVIF_PIXEL_FORMAT_YUV420);
    if (!image) return 1;
    image->colorPrimaries = AVIF_COLOR_PRIMARIES_BT709;
    image->transferCharacteristics = AVIF_TRANSFER_CHARACTERISTICS_SRGB;
    image->matrixCoefficients = AVIF_MATRIX_COEFFICIENTS_BT601;
    image->yuvRange = AVIF_RANGE_FULL;
    avifRGBImageSetDefaults(&rgb, image);
    rgb.format = AVIF_RGB_FORMAT_RGBA;
    if (avifRGBImageAllocatePixels(&rgb) != AVIF_RESULT_OK) return 1;
    for (uint32_t y = 0; y < size; ++y) {
        uint8_t *row = rgb.pixels + y * rgb.rowBytes;
        for (uint32_t x = 0; x < size; ++x) {
            row[x * 4 + 0] = (uint8_t)x;
            row[x * 4 + 1] = (uint8_t)y;
            row[x * 4 + 2] = (uint8_t)((x + y) / 2);
            row[x * 4 + 3] = 255;
        }
    }
    if (avifImageRGBToYUV(image, &rgb) != AVIF_RESULT_OK) return fprintf(stderr, "RGBToYUV failed\n"), 1;

    avifEncoder *encoder = avifEncoderCreate();
    if (!encoder) return 1;
    encoder->quality = 85;
    encoder->speed = 6;
    encoder->maxThreads = 4;
    if (avifEncoderSetCodecSpecificOption(encoder, "c:tune", "ssim") != AVIF_RESULT_OK) return 1;
    avifRWData output = AVIF_DATA_EMPTY;
    avifResult result = avifEncoderWrite(encoder, image, &output);
    if (result != AVIF_RESULT_OK) return fprintf(stderr, "encode failed: %s\n", avifResultToString(result)), 1;
    int ok = output.size > 12 && memcmp(output.data + 4, "ftypavif", 8) == 0;
    printf("encoded %zu bytes, ftyp ok: %d\n", output.size, ok);
    avifRWDataFree(&output);
    avifEncoderDestroy(encoder);
    avifRGBImageFreePixels(&rgb);
    avifImageDestroy(image);
    return ok ? 0 : 1;
}
