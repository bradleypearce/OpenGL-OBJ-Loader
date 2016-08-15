#version 330   
                                                                     
                                                                                   

const int MAX_POINT_LIGHTS = 1;                                                     
                                                                                   
in vec2 TexCoord0;                                                                 

in vec3 Normal0;                                                                   

in vec3 WorldPos0;                                                                  
                                                                                  

out vec4 FragColor;                                                                 
                                                                                   

/// Our base light variables are required for each type of type
/// these are our colour, diffuse and ambient variables which 
/// are required for an accurate blinn-phong model. 
struct BaseLight                                                                  

{                                                                                   
   
 	vec3 Color;                                                                     
   
 	float AmbientIntensity;                                                         
   
 	float DiffuseIntensity;                                                         

};                                                                                  
                                                                                  



/// Our directional light only requires a direction 
/// other than our baselight struct. We use this direction
/// to represent the direction our directional light will 
/// illuminate in.
struct DirectionalLight                                                            

{                                                                                   
   
 	BaseLight Base;                                                                 
    
	vec3 Direction;                                                                 

};   

/// Attenuation is essential for lights to look like their 
/// light becomes more faded relative to it's position. We 
/// define 3 different variables to represent our different types 
/// of attenuation, constant (light doesn't drop of), linear (light 
/// drops of relative to the distance from the source and exponential
/// (light drops of relative to distance squared).                                                                                
                                                                                    

struct Attenuation                                                                 

{                                                                                   
   
 	float Constant;                                                                 
   
 	float Linear;                                                                   
   
 	float Exp;                                                                      

};                                  

/// Our point light simply contains a position, a base light 
/// and an attenuation struct. This is all we require to get 
/// the effect of our point light required.                                                 
                                                                                    

struct PointLight                                                                          
{                                                                                           
  
  	BaseLight Base;                                                                         
    
	vec3 Position;                                                                          
    
	Attenuation Atten;                                                                      

};  

/// Our spot light requires a point light struct 
/// as it needs to make use of the attenuation, the position
/// and our base light attributes. It makes more sense for it 
/// to contain a pointlight to contain these attributes. We also 
/// define a direction the spot light will illuminate and a cutoff
/// to represent the angle of our cone of light. 
struct SpotLight
{
	PointLight Base;
	vec3 Direction;
	float Cutoff;
};
                                                                                        
                                                                                                                                                          
uniform DirectionalLight gDirectionalLight;                                                
uniform PointLight PointLights[1]; 
uniform SpotLight SpotLights[1];                                         

uniform sampler2D gSampler;                                                                
uniform vec3 gEyeWorldPos;                                                                 

uniform float gMatSpecularIntensity;                                                       
uniform float gSpecularPower; 

/// This function takes in a baselight struct, a direction the light is illuminating 
/// and our model's (current) normal. We then define an ambient colour which we set 
/// equal to the colour of our light (passed in as a uniform) and multiply this vector
/// by our ambient intensity uniform, giving us our ambient colour. We then define a 
/// diffuse factor (the cosine of the angle between the light vector and the current normal).
/// Then if our diffuse factor is more than zero, we calculte our diffuse colour as our light's 
/// colour multiplied by our diffuse intensity and our diffuse factor. Resulting in our light's 
/// diffuse colour. We then calculate a vector which represents the space between the vertex and 
/// the eye. Along with this, we also calculate our reflected light which is a normalized vector
/// of the reflected vector between the direction of our light and our current normal. Then we get the
/// the specular factor by assigning as the cosine of the angle between our vertextoeye and our reflected light.
/// If our specular colour is more than 0 then we set our specular factor to the power of our specular power 
/// and our specular colour as our light's colour multiplied by our specular intensity uniform multiplied by our 
/// squared specular factor. We then return our ambient multiplied by our diffuse multiplied by our specular colour. 
/// The end result makes our model's appear as if they are reflecting light, have a diffuse colour and an ambient colour
/// relative to the lights in our scene.                                                               
                                                                                          
vec4 CalcLightInternal(in BaseLight Light, in vec3 LightDirection, in vec3 Normal)                  

{                                                                                           
    
	vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    
	float DiffuseFactor = dot(Normal, -LightDirection);                                    
                                                                                            
   
 	vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    

	vec4 SpecularColor = vec4(0, 0, 0, 0);  
                                                
                                                                                            
    
	if (DiffuseFactor > 0) 
	{                                                                
        
		DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);    
                                                                                            
        
		vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             
        
		vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
       
		float SpecularFactor = dot(VertexToEye, LightReflect);                                      
      
  		if (SpecularFactor > 0)
 		{                                                           
            
			SpecularFactor = pow(SpecularFactor, gSpecularPower);
            
			SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        
		}                                                                                   
    
	}   
                                                                                    
                                                                                            
    
	return (AmbientColor + DiffuseColor + SpecularColor);                                  

}

/// This function simply takes in our current normal and returns our 
/// calclightinternal function using the directional light's base and direction variables  
/// along with our current normal.                                                                                        
                                                                                            

vec4 CalcDirectionalLight(in vec3 Normal)                                                     

{                                                                                           
   
 	return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal); 
}    
 
/// To calculate our pointlights, we take in our point light and our current normal.
/// Then define a direction vector which is equal to our current vertex's current world
/// position minus the light's position. Then to find the distance from this vector is 
/// from the world position, we get the length of our light direction vector. Then we normalize 
/// our light direction vector for use in our calclightinternal function. We then define our colour
/// vector which we set equal to the vector our calclighinternal function returns (using our pointlight's
/// base attribute, our lightdirection vector and our current normal as parameters). Then we define our 
/// attenuation which we set equal to our each of our different types of attenuation (using distance and distance squared
/// for both our linear and our exponential uniforms). Then once we have the sum of our attenuation uniforms, we return the 
/// the vector equal to colour divided by attenuation. This gives the effect of our light's brightness get dimmer the further
/// from the source it is.                                                                                       
                                                                                           
 
vec4 CalcPointLight(in struct PointLight l, in vec3 Normal)                                                 

{                                                                                           
    
	vec3 LightDirection = WorldPos0 - l.Position;                         
    
	float Distance = length(LightDirection);                                                
   
 	LightDirection = normalize(LightDirection);                                             
                                                                                            
    
	vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);       
    
	float Attenuation =  l.Atten.Constant +                               
                         
	l.Atten.Linear * Distance +                      
                         
	l.Atten.Exp * Distance * Distance;               
                                                                                            
   
 	return Color / Attenuation;                                                             

}    

/// This function takes in our spotlight and our current normal as a parameter. 
/// We first of all get the vector of our current vertex's world position minus 
/// the spotlight's position uniform, giving us how far away the light is from the 
/// vertex (normalized). Then to get the illumination of our spotlight, we get the cosine of the angle
/// between the vertex to light source vector and the direction of our light. If the spotlight factor 
/// is more than our cutoff angle, we define our colour vector to the value of our calcpointlight function, 
/// passing in our base light (pointlight attributes) and our current normal. We then return colour multiplied
/// by the ratio of our spotlight factor multiplied by 1 divided by the ratio of our cutoff angle. This gives us 
/// the contribution of our spotlight reletive to our eye position. If the spotlight factor is less than our cutoff 
/// angle then we return a zero vector as our spotlight is not contributing to our scene's lighting colour. 
vec4 CalcSpotLight(in SpotLight l, in vec3 Normal)
{
	vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);
	float SpotFactor = dot(LightToPixel, l.Direction);

	if(SpotFactor > l.Cutoff)
	{
		vec4 Color = CalcPointLight(l.Base, Normal);
		return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff)); 
	}
	else
	{
		return vec4(0,0,0,0);
	}
}

/// In main, we normalize our current normal (for use in our lighting calculations). 
/// We then define our totallight vector and calculate our scene's directional light 
/// passing in our current normal. We then use a for loop to calculate our pointlights
/// spot lights. My original intention was to loop reletive to the size of our pointlight 
/// and spotlight arrays and calculate them in order using i as an index. The issue i could 
/// not fix was unfortunately, when getting our uniform locations, we couldn't assign our 
/// individual uniforms using an index. Unfortunately, i did not have enough to combat this problem 
/// so i simply added one point light and one spotlight uniform and loop round the loop once. Passing in
/// our lights into each function and our current Normal. The resultant fragcolour is our texture sample 
/// colour multiplied by our total light vector.                                                                                        
                                                                                            

void main()                                                                                 

{                                                                                           
   
 	vec3 Normal = normalize(Normal0);  
                                                     
   
 	vec4 TotalLight = CalcDirectionalLight(Normal);  
        
	for (int i = 0 ; i < 1;i++)
	{                                           
        
		TotalLight += CalcPointLight(PointLights[i], Normal);   
		TotalLight += CalcSpotLight(SpotLights[i], Normal);                                         
    
	}                                            
                                                                                            
         
                                                                     
                                                                                            
    
	FragColor = vec4(texture2D(gSampler, TexCoord0).rgb, 1.0f) * TotalLight;                           

}
