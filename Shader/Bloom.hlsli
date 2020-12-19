
cbuffer CONSTANT_BUFFER : register(b0)
{
    float glow_extraction_threshold;
    float blur_convolution_intensity;
    float lens_flare_threshold;
    float lens_flare_ghost_dispersal;
    int number_of_ghosts;
    float lens_flare_intensity;
    int2 options;
}

// if you change value of 'number_of_downsampled', you must change 'number_of_downsampled' in bloom.h to this same value.
const static uint number_of_downsampled = 3;
