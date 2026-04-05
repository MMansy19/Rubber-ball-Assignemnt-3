You are a senior C++ graphics engineer with strong experience in OpenGL (GLEW + FreeGLUT) and physics-based simulations.

I will provide you with an existing OpenGL C++ program that simulates a thrown ball using basic projectile motion equations. Your task is to EXTEND and MODIFY this code to implement a realistic rubber (bouncing) ball simulation.

========================
🎯 OBJECTIVE
========================
Transform the current “throwing ball” animation into a physically accurate bouncing ball system.

The ball MUST:
1. Bounce when it hits the ground (y = 0 plane).
2. Lose height after each bounce (simulate energy loss).
3. Eventually come to rest (stop bouncing completely).

========================
⚠️ STRICT REQUIREMENTS
========================
1. You MUST continue using the physics equations of motion:
   - Horizontal: x = h * t
   - Vertical: y = v * t - (1/2) * g * t²

2. DO NOT replace the motion system with a completely different approach.
3. You MAY introduce additional physics variables such as:
   - coefficient of restitution (e.g., 0.6–0.9)
   - velocity updates after collision
   - bounce damping logic

========================
🧠 PHYSICS LOGIC TO IMPLEMENT
========================
- Detect collision with the ground:
  IF y <= 0 → collision حدث

- On collision:
  - Reverse vertical velocity
  - Apply damping:
    v = -v * restitution

- Reset time (t = 0) after each bounce
- Maintain horizontal motion continuously (no reset for x)

- Stop condition:
  If |v| becomes very small (e.g., < 0.1), stop bouncing and keep the ball on the ground

========================
🛠️ IMPLEMENTATION DETAILS
========================
You must:
1. Refactor the code cleanly (do NOT make hacks)
2. Introduce clear variables:
   - float restitution
   - float currentY, currentX
   - float currentVelocityY

3. Modify ONLY what is necessary:
   - drawScene()
   - animate()

4. Ensure:
   - Smooth animation
   - No jittering at ground
   - Stable stopping condition

========================
🎮 EXISTING FEATURES TO PRESERVE
========================
Keep all current interactions working:
- Space → toggle animation
- Arrow keys → control velocity
- Page up/down → gravity
- R → reset

========================
✨ BONUS (Optional but preferred)
========================
- Add a ground line for visual clarity
- Clamp ball so it never visually goes below ground
- Improve realism slightly without breaking the rules

========================
📦 OUTPUT FORMAT
========================
Return:
1. ✅ FULL updated C++ code (ready to compile)
2. ✅ Clear inline comments explaining physics logic
3. ✅ Short explanation of how bouncing works in your implementation

========================
🚫 DO NOT
========================
- Do not remove OpenGL structure
- Do not oversimplify the physics
- Do not ignore the equations requirement

========================
Here is the original code:
[PASTE THE CODE HERE]
========================