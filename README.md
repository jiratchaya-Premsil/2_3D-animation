# 2_3D-animation

## writing 3D animation where i create terraria-like platform by using
glm::two_pi<float>() 
created arrays of position made of X,Y,Z

## and create cube for each position before resize to make tall cube
glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

glm::translate(model, pos);
glm::scale(model, glm::vec3(0.2f, 1.2f, 0.2f));

## use time and sin() to make cube sways
float sway = sin(time + pos.x * 0.5f + pos.z * 0.5f) * 0.4f;

## change color of cube by the Y and how close from cube to camera

 glm::vec4 finalColor = glm::mix(baseColor, closeColor, t);


## make the cube bend when closer to the camera


float t = glm::clamp( 1.0f - (dist - nearDist) / (farDist - nearDist),0.0f, 1.0f);


glm::vec3 finalAxis = glm::normalize( glm::mix(windAxis, bendAxis, t*2.0f));


float angle = sway * (1.0f - t) + bendStrength;


https://github.com/user-attachments/assets/72ebcd9b-4b8e-4ec0-917c-d95a14702d11

