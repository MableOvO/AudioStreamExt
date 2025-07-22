#ifndef WaveformDataChannel_H
#define WaveformDataChannel_H


#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>




#pragma once

namespace godot {
    class WaveformData;
}




namespace godot {

    class WaveformDataChannel : public RefCounted {
        GDCLASS(WaveformDataChannel, RefCounted);

    
    protected:
        static void _bind_methods();

    public:
        Ref<WaveformData> parent;
        int channelID;

        void create(Ref<WaveformData>,int);

        inline float minSample(int);
        inline float maxSample(int);

        inline float minSampleMap(int);
        inline float maxSampleMap(int);


        float minSampleRange(int,int);
        inline float minSampleRangeMap(int,int);
        
        float maxSampleRange(int,int);
        inline float maxSampleRangeMap(int,int);


        //i dont ever see these being used, but they were in the original Waveform renderer from Funkin
/*         void setMinSample(int,int);
        void setMaxSample(int,int); */
    };

}


#endif