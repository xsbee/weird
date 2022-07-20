#include <random>

struct ilac0r
{
    unsigned int m_width;
    unsigned int m_height;
    
    uint32_t *m_frame_buffer;
    size_t m_frame_size;

    bool m_odd_frame = false;

    ilac0r(unsigned int width, unsigned int height)
        : m_width(width), m_height(height),
          m_frame_size(static_cast<size_t>(width) * height)
    {
        this->m_frame_buffer = new uint32_t[m_frame_size];
    }

    void process(const uint32_t *inframe, uint32_t *outframe)
    {
        for (unsigned int j = 0; j < m_height; j += 2)
        {
            for (unsigned int i = 0; i < m_width; i += 2)
            {
                size_t p0 =  j    * m_width + i + m_odd_frame,
                       p1 = (j+1) * m_width + i + m_odd_frame;

                m_frame_buffer[p0] = inframe[p0];
                m_frame_buffer[p1] = inframe[p1];
            }
        }

        std::copy(m_frame_buffer, m_frame_buffer + m_frame_size, outframe);
        m_odd_frame = !m_odd_frame;
    }

    ~ilac0r() {
        delete[] m_frame_buffer;
    }
};

extern "C" {
#include <frei0r.h>

int f0r_init()
{
    return 0;
}

f0r_instance_t f0r_construct(unsigned int width, unsigned int height)
{
    ilac0r *instance = new ilac0r((width / 2) * 2, (height / 2) * 2);

    return instance;
}

void f0r_get_plugin_info(f0r_plugin_info_t *info)
{
    info->name = "ilac0r";
    info->author = "xsbee";
    info->explanation = "Horizontal and vetical interlacer";
    info->major_version = 1;
    info->minor_version = 0;
    info->frei0r_version = FREI0R_MAJOR_VERSION;

    info->color_model = F0R_COLOR_MODEL_PACKED32;
    info->plugin_type = F0R_PLUGIN_TYPE_FILTER;
    info->num_params = 0;
}

void f0r_get_param_info	(f0r_param_info_t *info, int param_index) {}

void f0r_get_param_value(
    f0r_instance_t instance,
    f0r_param_t param,
    int param_index 
) {}

void f0r_set_param_value(
    f0r_instance_t instance,
    f0r_param_t param,
    int param_index 
) {}

void f0r_update	(
    f0r_instance_t instance,
    double,
    const uint32_t *inframe,
    uint32_t *outframe 
)
{
    static_cast<ilac0r*>(instance)->process(inframe, outframe);
}

void f0r_deinit() {}

void f0r_destruct (f0r_instance_t instance)	
{
    delete static_cast<ilac0r*>(instance);
}
}