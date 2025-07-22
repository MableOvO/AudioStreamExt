#include "WaveFormDataChannel.h"


#include "WaveformData.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void WaveformDataChannel::create(Ref<WaveformData> thisParent,int thisID)
{
	parent = thisParent;
	channelID = thisID;
}



inline float WaveformDataChannel::minSample(int i)
{
    int offset = (i * parent->channels + channelID) * 2;
    return parent->Get(offset);
    
}


inline float WaveformDataChannel::maxSample(int i)
{
    int offset = (i * parent->channels + channelID) * 2 + 1;
    return parent->Get(offset);
}



inline float WaveformDataChannel::minSampleMap(int i)
{
    return minSample(i) / parent->maxSampleValue();
}


inline float WaveformDataChannel::maxSampleMap(int i)
{
    return maxSample(i) / parent->maxSampleValue();
}


float WaveformDataChannel::minSampleRange(int start,int end)
{
	float minS = parent->maxSampleValue();
	
	for (int i = start; i < end; ++i)
    {
		float sample = minSample(i);
		if (sample < minS){
            minS = sample;
        }
    }
	return minS;
}



float WaveformDataChannel::maxSampleRange(int start,int end)
{
	float maxS = -(parent->maxSampleValue());



    
	for (int i = start; i < end; ++i)
    {
        float sample = maxSample(i);
		if (sample > maxS) {
            maxS = sample;
        }
    }
	return maxS;
}






inline float WaveformDataChannel::minSampleRangeMap(int start,int end)
{
    return minSampleRange(start, end) / parent->maxSampleValue();
}
	





inline float WaveformDataChannel::maxSampleRangeMap(int start,int end)
{
    return maxSampleRange(start, end) / parent->maxSampleValue();
}





void WaveformDataChannel::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create", "parent", "channel_id"), &WaveformDataChannel::create);
    
    ClassDB::bind_method(D_METHOD("minSample", "i"), &WaveformDataChannel::minSample);
    ClassDB::bind_method(D_METHOD("maxSample", "i"), &WaveformDataChannel::maxSample);
    
    ClassDB::bind_method(D_METHOD("minSampleMap", "i"), &WaveformDataChannel::minSampleMap);
    ClassDB::bind_method(D_METHOD("maxSampleMap", "i"), &WaveformDataChannel::maxSampleMap);
    
    ClassDB::bind_method(D_METHOD("minSampleRange", "start", "end"), &WaveformDataChannel::minSampleRange);
    ClassDB::bind_method(D_METHOD("maxSampleRange", "start", "end"), &WaveformDataChannel::maxSampleRange);

    ClassDB::bind_method(D_METHOD("minSampleRangeMap", "start", "end"), &WaveformDataChannel::minSampleRangeMap);
    ClassDB::bind_method(D_METHOD("maxSampleRangeMap", "start", "end"), &WaveformDataChannel::maxSampleRangeMap);
}
