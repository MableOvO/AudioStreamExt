#include "AudioStreamEXT.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


#define OGG_IMPL
#define VORBIS_IMPL
#include "minivorbis.h"

using namespace godot;


//essentially just https://gist.github.com/mattmalec/6ceee1f3961ff3068727ca98ff199fab but JUST the goods
//downsample our PCM data if the sample isnt 44100
PackedFloat32Array AudioStreamEXT::downsample(PackedFloat32Array Data,float length,int isMono,int inFrequency) {
    int downsampleFreq = 44100;

    if (int(inFrequency) == downsampleFreq)
    {
        ERR_PRINT(vformat("SAMPLE FREQ IS ALREADY %s, skipping downsample", downsampleFreq));
        return Data.slice(0,length);
    }
    
    print_line(vformat("downsampling stream from %s to %s", inFrequency,downsampleFreq));
    float scale = float(downsampleFreq) / float(inFrequency);
    PackedFloat32Array output;

    float pos = 0.0;
    int out_pos = 0;


    int output_len = int(length * scale);


    if (isMono)
    {
        float sum = 0.0;
        int in_pos = 0;

        while (out_pos < output_len && in_pos < length)
        {
            float first_val = Data[in_pos];
            in_pos += 1;
            float next_pos = pos + scale;
            if (next_pos >= 1.0)
            {
				sum += first_val * (1.0 - pos);
				output.append(sum);
				out_pos += 1;
				next_pos -= 1.0;
				sum = next_pos * first_val;
            }
            else
            {
               sum += scale * first_val;
            }
			pos = next_pos;
        }
    }
    else 
    {
		float sum_l = 0.0;
		float sum_r = 0.0;
		int in_pos = 0;

		while (out_pos < output_len && in_pos < length)
        {
			float left = Data[in_pos];
			float right = Data[in_pos + 1];
			in_pos += 2;
			float next_pos = pos + scale;

			if (next_pos >= 1.0)
            {
				sum_l += left * (1.0 - pos);
				sum_r += right * (1.0 - pos);
				output.append(sum_l);
				output.append(sum_r);
				out_pos += 2;
				next_pos -= 1.0;
				sum_l = next_pos * left;
				sum_r = next_pos * right;
            }
			else 
            {
				sum_l += scale * left;
				sum_r += scale * right;
            }

			pos = next_pos;

        }



    }
    return output;
}






//get bits 
int AudioStreamEXT::GetBitsPerSample(float duration, int sampleRate, int channels,int PCM_BYTES)
{
    if (duration <= 0 or sampleRate <= 0 or channels <= 0)
    {
        ERR_PRINT("Something went wrong calculating bit depth :/");
        return -1;
    }


    float total_samples = duration * sampleRate * channels;
    float bits_per_sample = (PCM_BYTES * 8.0) / total_samples;

    return round(bits_per_sample);
}









//i will EAT... YOUR... HEARTT
struct MemoryOgg {
    const uint8_t* data = nullptr;
    size_t size = 0;
    size_t pos = 0;
};

size_t mem_read_func(void* ptr, size_t size, size_t nmemb, void* datasource) {
    MemoryOgg* mem = static_cast<MemoryOgg*>(datasource);
    size_t remaining = mem->size - mem->pos;
    size_t to_read = size * nmemb;
    if (to_read > remaining) {
        to_read = remaining;
    }
    memcpy(ptr, mem->data + mem->pos, to_read);
    mem->pos += to_read;
    return to_read / size;
}

int mem_seek_func(void* datasource, ogg_int64_t offset, int whence) {
    MemoryOgg* mem = static_cast<MemoryOgg*>(datasource);
    size_t new_pos = 0;
    switch (whence) {
        case SEEK_SET: new_pos = offset; break;
        case SEEK_CUR: new_pos = mem->pos + offset; break;
        case SEEK_END: new_pos = mem->size + offset; break;
        default: return -1;
    }
    if (new_pos > mem->size) return -1;
    mem->pos = new_pos;
    return 0;
}

long mem_tell_func(void* datasource) {
    return static_cast<MemoryOgg*>(datasource)->pos;
}

int mem_close_func(void* datasource) {
    return 0;
}






//decode the ogg into raw pcm data
Dictionary AudioStreamEXT::DecodeOggMem(String OggPath) 
{

    #define BUFFER_SIZE 4096
    #define BUFFER_READ_TYPE 1
    //0 for Little-Endian, 1 for Big-Endian 
    //im jus gonna assume big endiean



    
    Ref<FileAccess> file = FileAccess::open(OggPath, FileAccess::READ);
    if (!file.is_valid()) {
        ERR_PRINT("Failed to open ogg, is ur path correct? " + OggPath);
        return Dictionary();
    }

    PackedByteArray ogg_data = file->get_buffer(file->get_length());
    file->close();


    //godot is weird about this stuff, so u gotta go the long way around to read files
    MemoryOgg* MEM = memnew(MemoryOgg);
    MEM->data = ogg_data.ptr();
    MEM->size = ogg_data.size();
    MEM->pos = 0;

    ov_callbacks callbacks;
    callbacks.read_func = mem_read_func;
    callbacks.seek_func = mem_seek_func;
    callbacks.close_func = mem_close_func;
    callbacks.tell_func = mem_tell_func;
    


    OggVorbis_File oggFile;


    //actually PROCESS the fucking file
    if(ov_open_callbacks(MEM, &oggFile, NULL, 0, callbacks) != 0) {
            ERR_PRINT("Invalid Ogg file " + OggPath);
            return Dictionary{};
    }

    vorbis_info *pInfo = ov_info(&oggFile,-1);

    //get info
    if (pInfo == NULL)
    {
        ERR_PRINT("Invalid ogg data " + OggPath);
        ov_clear (&oggFile);
        return Dictionary{};
    }

    int channels = pInfo->channels;
    float sampleRate = pInfo->rate;
    int bitsPerSample = 16;

    ogg_int64_t total_PCM = ov_pcm_total (&oggFile, -1);
    int dataLength = total_PCM * channels * bitsPerSample / 8;

    

    PackedFloat32Array PCM_DATA;
    int bitStream;

    float** pcm_channels = nullptr;
    long samples;


    while ((samples = ov_read_float(&oggFile, &pcm_channels, 4096, &bitStream)) > 0) {
        for (long i = 0; i < samples; ++i) {
            for (int c = 0; c < channels; ++c) {
                PCM_DATA.push_back(pcm_channels[c][i]);
            }
        }
    }


    bool isMono = false;
    if (channels == 1){isMono = true;} 

    PCM_DATA = downsample(PCM_DATA,PCM_DATA.size(),isMono,sampleRate);
    sampleRate = 44100;


    //cleanup
    ov_clear (&oggFile);

    float OGG_DURATION = (PCM_DATA.size() * 1000 / sampleRate) / channels;

    OGG_DURATION = Math::snapped(OGG_DURATION,1.0) / 1000;

    bitsPerSample = GetBitsPerSample(OGG_DURATION,sampleRate,channels,PCM_DATA.size());
    
    //send info
    Dictionary result;
    result["pcm_data"] = PCM_DATA;
    result["sample_rate"] = sampleRate;
    result["channels"] = channels;
    result["OGG_DURATION"] = OGG_DURATION;
    result["bitsPerSample"] = bitsPerSample;

    return result;
}


















void AudioStreamEXT::_bind_methods() {
    ClassDB::bind_static_method("AudioStreamEXT", D_METHOD("GetBitsPerSample", "duration","sampleRate","channels","PCM_BYTES"), &AudioStreamEXT::GetBitsPerSample);

    ClassDB::bind_static_method("AudioStreamEXT", D_METHOD("DecodeOggMem", "OggPath"), &AudioStreamEXT::DecodeOggMem);
    ClassDB::bind_static_method("AudioStreamEXT", D_METHOD("downsample", "Data", "length", "isMono", "inFrequency"), &AudioStreamEXT::downsample);
}