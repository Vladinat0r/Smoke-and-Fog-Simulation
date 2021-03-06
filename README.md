# Smoke-and-Fog-Simulation
This simulation is written in C++ with OpenGL API and features drone flight, day/night cycle, octree collision detection, point/spotlight/directional lighting, shadows, smoke and fog.

Controls:
    ’W’, ’A’, ’S’, ’D’ - hold ’W’ to accelerate in the direction of the camera.
    ’A’ and ’D’ to strafe and ’S’ to reverse.
    ’SPACE’ - hold key to ascend.
    ’CTRL’ - hold key to descend.
    ’SHIFT’ - hold key for a speed boost when accelerating forward, doubles the speed limit and triples the acceleration.
    ’F’ - press key to toggle spotlight.
    ’G’ - hold key to descend and land, the drone will level itself out automatically on landing and shut off the engine, disabling movement controls and hovering animation.
    ’E’-press key to start the engine, the drone will take off and controls and hovering animation will be re-enabled.
    ’+’ and ’-’ - press to zoom camera in and out.
    ’1’ - press key to switch to front facing camera below the drone.
    ’2’ - press key to switch to front facing camera on top of the drone.
    ’3’ - press key to switch camera to the third person view.
    ’4’ - press key to switch camera to the drone controller’s point of view.
    ’5’ - press key to switch camera to the top-down view of the drone.
    ’6’ - press key to switch camera to the rear facing camera below the drone.
    ’X’ - press key to disable/enable smoke rendering.
    ’C’ - press key to pause/continue the day and night cycle.
    ’V’ - press key to disable/enable collision detection.
    ’B’ - press key to toggle the octree bounding box outlines.
