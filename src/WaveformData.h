#ifndef WaveformData_H
#define WaveformData_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "WaveFormDataChannel.h"








namespace godot {

    class WaveformData : public RefCounted {
        GDCLASS(WaveformData, RefCounted)

    protected:
        static void _bind_methods();



    public:
        int version = -1;
        int channels = 1;
        int sampleRate = 44100;
        int samplesPerPoint = 256;
        int bits = 16;
        int length = 0;

        std::vector<float> data;
        Array channelData;

        int songLength = 0;

        int _maxSampleValue = 0;




        void create(int, int,int,int,int,int, PackedFloat32Array); //initializer 9000!!
        Array buildChannelData();
        Ref<WaveformDataChannel> channel(int);

        inline float Get(int);
        inline void Set(int,float);

        int maxSampleValue();
        inline float lenSamples();
        inline float lenSeconds();
        inline int secondsToIndex(float);
        inline float indexToSeconds(int);
        inline float pointsPerSecond();

        inline int percentToIndex(float);
        inline float indexToPercent(int);

        //getters n setters
        PackedFloat32Array get_data();
        void set_data(PackedFloat32Array);
        void clear_data() {data.clear();}

        int get_songLength() const { return songLength; }
        void set_songLength(const int &p_songLength) { songLength = p_songLength; }
    };
}
#endif 