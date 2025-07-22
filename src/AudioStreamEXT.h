#ifndef AudioStreamEXT_H
#define AudioStreamEXT_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "minivorbis.h"

//i do not know C++ glob help me !!!!!
namespace godot {

    class AudioStreamEXT : public Object {
        GDCLASS(AudioStreamEXT, Object)

    protected:
        static void _bind_methods();

    public:
        static int GetBitsPerSample(float,int,int,int);
        static Dictionary DecodeOggMem(String);
        static PackedFloat32Array downsample(PackedFloat32Array,float,int,int);
    };

}
#endif