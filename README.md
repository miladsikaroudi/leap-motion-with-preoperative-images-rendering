# Interacting with 3D Objects in VTK using Leap Motion

This project demonstrates how to integrate Leap Motion's hand tracking capabilities with the Visualization Toolkit (VTK) for interactive 3D object manipulation. With this setup, users can interact with 3D scenes using hand gestures, allowing for a more immersive and intuitive experience.

## Capabilities

- **Real-time hand tracking**: Utilizing the Leap Motion API, the project tracks the movement and gestures of the user's hands in real-time.
- **3D object manipulation**: The tracked hand data is used to manipulate 3D objects within the VTK scene. The user can rotate the 3D objects along the x, y, and z axes based on hand movements.
- **Gesture recognition**: The application responds to specific Leap Motion gestures, allowing the user to change the mode of interaction.

## How it Works

The setup can be divided into three major components: Leap Motion tracking, VTK visualization, and the integration of the two.

1. **Leap Motion Tracking**: A Leap Motion Listener class (`SampleListener`) is defined to process frames received from the Leap Motion device. The listener tracks the position and orientation of the user's hands and responds to specific gestures. When two hands are detected, the 3D object freezes in its current state. When a valid gesture is detected, the mode of interaction changes.

2. **VTK Visualization**: The VTK setup consists of a DICOM Image Reader for reading 3D image data, a Volume Mapper for rendering the 3D volume, and a Volume Property for setting visual properties like color and opacity. The visualization also includes a Text Actor to display the current mode on the screen.

3. **Integration**: The Leap Motion data is incorporated into the VTK visualization using a timer callback. In each callback, the 3D object is rotated based on the current hand orientation. The camera position is also updated to follow the user's hand movements. The current interaction mode and background color of the text actor are updated based on the recognized gestures.

## Leveraging the Project

This project serves as a basic framework for creating interactive 3D applications using Leap Motion and VTK. It can be extended in various ways:

- **New interaction modes**: Additional modes can be added for different types of interaction, such as scaling or translating the 3D object.
- **More complex gestures**: The Leap Motion API supports a wide variety of gestures, including swipes, circles, and pinches. These can be used to add more complex controls.
- **User interface elements**: VTK supports various types of actors and widgets that can be used to create a full-fledged user interface.
- **Integration with other systems**: The project can be integrated with other systems for applications like virtual reality, augmented reality, or teleoperation.

For detailed instructions on how to run and modify the code, please refer to the included README file.
