#include "WaveformData.h"
#include "WaveFormDataChannel.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


using namespace godot;

//promise i like it like that!

void WaveformData::create(int this_version, int this_channels,int this_sampleRate,int this_samplesPerPoint,int this_bits,int this_length, PackedFloat32Array this_data)
{
 	version = this_version;
	channels = this_channels;
	sampleRate = this_sampleRate;
	samplesPerPoint = this_samplesPerPoint;
	bits = this_bits;
	length = this_length;

    set_data(this_data);
    buildChannelData();
    maxSampleValue();


}


Array WaveformData::buildChannelData()
{
    channelData.clear();
    for (int i = 0; i < channels; i++)
    {
        Ref<WaveformDataChannel> newWaveForm = memnew(WaveformDataChannel);
        //hopefully ts works
        newWaveForm->create(this, i);
        channelData.append(newWaveForm);
    }
    return channelData;
}


Ref<WaveformDataChannel> WaveformData::channel(int index)
{
    if (channelData.is_empty())
    {
        buildChannelData();
    }
    if (index >= 0 and index < channelData.size())
    {
        return channelData[index];
    }
	else
    {
        Ref<WaveformDataChannel> newWaveForm = memnew(WaveformDataChannel);
        return newWaveForm;
    }
		
}



inline float WaveformData::Get(int index)
{
    if (index >= 0 and index < data.size())
       {
         return data[index];
       }
    else
        {
            return 0.0;
        }
}   



inline void WaveformData::Set(int index,float value)
{
    if (index >= 0 && index < data.size())
    { 
        data[index] = value;
    }
    else
    {
        ERR_PRINT("waveform set index out of range!");
    }
}

int WaveformData::maxSampleValue(){
    if (_maxSampleValue != 0)
        {
            return _maxSampleValue;
        }
    
    _maxSampleValue = int(pow(2, bits));
    return _maxSampleValue;
}





inline float WaveformData::lenSamples(){
    return length * samplesPerPoint;
}
	
inline float WaveformData::lenSeconds(){
    return lenSamples() / sampleRate;
}

inline int WaveformData::secondsToIndex(float seconds){
    return int(seconds * pointsPerSecond());
}
	

inline float WaveformData::indexToSeconds(int index)
{
    return index / pointsPerSecond();
}

inline float WaveformData::pointsPerSecond()
{
	return sampleRate /  samplesPerPoint;
}

inline int WaveformData::percentToIndex(float percent)
{
    return int(percent * length);
}

inline float WaveformData::indexToPercent(int index)
{
    return index / length;
}




//packedfloat32Arrays are slow in C++ sooo CONVERT TO std::vector !!!!!
PackedFloat32Array WaveformData::get_data()
{
    PackedFloat32Array array;
    array.resize(data.size());
    for (int i = 0; i < data.size(); ++i) {
        array[i] = data[i];
    }
    return array;
}
void WaveformData::set_data(PackedFloat32Array array) {
    data.resize(array.size());
    for (int i = 0; i < array.size(); ++i) {
        data[i] = array[i];
    }
}



void WaveformData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create", "version", "channels", "sample_rate", "samples_per_point", "bits", "length", "data"), &WaveformData::create);
    
    ClassDB::bind_method(D_METHOD("buildChannelData"), &WaveformData::buildChannelData);
    ClassDB::bind_method(D_METHOD("channel", "index"), &WaveformData::channel);
    
    ClassDB::bind_method(D_METHOD("Get", "index"), &WaveformData::Get);
    ClassDB::bind_method(D_METHOD("Set", "index", "value"), &WaveformData::Set);

    ClassDB::bind_method(D_METHOD("maxSampleValue"), &WaveformData::maxSampleValue);
    ClassDB::bind_method(D_METHOD("lenSamples"), &WaveformData::lenSamples);
    ClassDB::bind_method(D_METHOD("lenSeconds"), &WaveformData::lenSeconds);
    ClassDB::bind_method(D_METHOD("secondsToIndex", "seconds"), &WaveformData::secondsToIndex);
    ClassDB::bind_method(D_METHOD("indexToSeconds", "index"), &WaveformData::indexToSeconds);
    ClassDB::bind_method(D_METHOD("pointsPerSecond"), &WaveformData::pointsPerSecond);
    ClassDB::bind_method(D_METHOD("percentToIndex", "percent"), &WaveformData::percentToIndex);
    ClassDB::bind_method(D_METHOD("indexToPercent", "index"), &WaveformData::indexToPercent);


    ClassDB::bind_method(D_METHOD("get_data"), &WaveformData::get_data);
    ClassDB::bind_method(D_METHOD("set_data", "data"), &WaveformData::set_data);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "data"), "set_data", "get_data");

    ClassDB::bind_method(D_METHOD("get_songLength"), &WaveformData::get_songLength);
    ClassDB::bind_method(D_METHOD("set_songLength", "songLength"), &WaveformData::set_songLength);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "songLength"), "set_songLength", "get_songLength");

    ClassDB::bind_method(D_METHOD("clear_data"), &WaveformData::clear_data);

}



















