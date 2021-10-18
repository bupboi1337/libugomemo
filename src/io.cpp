#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>

#include "io.hpp"
#include "types.hpp"

/**
 * Read file into a vector
 *
 * Parameters:
 * - path: the path of the file to be read
 * Returns:
 * - the file contents as a u8 vector
 */
std::vector<u8> readFile(std::string path) {
    std::ifstream file(path, std::ios::binary);

    std::vector<u8> output;

    if (file) {
        // Stop eating newlines and whitespace in binary mode
        file.unsetf(std::ios::skipws);

        file.seekg(0, std::ios::end);
        std::streamoff file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        output.reserve(file_size);
        output.insert(output.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());

        // TODO: (low priority) add automatic gzip decompression support here

        file.close();
    }

    return output;
}

/**
 * Write s16 PCM audio data to a .wav file at the location specified.
 *
 * Can be used to write either kwz or ppm decoded audio, the sample rate is the only adjustment needed.
 *
 * Parameters:
 * - path: the path that the file will be written to
 * - audio: the audio data to be written
 * - sample_rate: the sample rate of the decoded audio:
 *      - KWZ: 16384
 *      - PPM: 8192
 */
void writeWAV(std::string path, std::vector<s16> audio, int sample_rate) {
    std::ofstream output_file(path, std::ios::binary);

    // Generate and write WAV header
    wav_hdr wav;
    wav.chunk_size = (u32)(audio.size() + 36);
    wav.subchunk_2_size = (u32)(audio.size() * 2);
    wav.sample_rate = sample_rate;
    wav.byte_rate = sample_rate * 2;
    output_file.write(reinterpret_cast<const char*>(&wav), sizeof(wav));

    // Write audio data
    output_file.write(reinterpret_cast<const char*>(&audio[0]), audio.size() * 2);

    output_file.close();
}

/**
 * Write a kwz file's thumbnail to a file
 *
 * kwz thumbnails are raw JPG data embedded in the file, so we just write that data to a file
 *
 * There is some garbage? data at the end of each file. This is still written because the DSi Library S3 bucket
 * stored thumbnails with this extra data, and it does not harm viewing ability due to the layout of JPG files
 *
 * Parameters
 * - buffer: the kwz file buffer as a u8 vector
 * - path: the file path to write the jpg file
 *     - be sure to include the .jpg extension
 * - start: where in the buffer the thumbnail starts
 * - size: the size of the thumbnail
 */
void kwzWriteThumbnail(std::vector<u8> buffer, std::string path, int start, int size) {
    std::ofstream output_file(path, std::ios::binary);

    output_file.write(reinterpret_cast<const char*>(&buffer[start]), size);

    output_file.close();
}