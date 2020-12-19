
cbuffer CAMERA_CONSTANTS : register(b10)
{
    row_major float4x4 rowCameraView;
    row_major float4x4 rowCameraProjection;
    row_major float4x4 rowCameraViewProjection;
    float4x4 cameraView;
    float4x4 cameraProjection;
    float4x4 cameraViewProjection;
    float4 lightDirection;
    
    float4 cameraPos;
    float4 cameraTarget;
    float4 cameraUp;
    float4 cameraRight;
    float4 elapsedTime;
};

