using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace StepDX
{
    public class Background
    {
        /// <summary>
        /// The DirectX device we will draw on
        /// </summary>
        private Device device = null;

        /// <summary>
        /// Background texture map
        /// </summary>
        private Texture backgroundT = null;

        /// <summary>
        /// Background vertex buffer
        /// </summary>
        private VertexBuffer backgroundV = null;


        public Background(Device device, float width, float height)
        {
            this.device = device;

            // Load the background texture image
            backgroundT = TextureLoader.FromFile(device, "../../../countryside.png");

            // Create a vertex buffer for the background image we will draw
            backgroundV = new VertexBuffer(typeof(CustomVertex.PositionColoredTextured), // Type
                4,      // How many
                device, // What device
                0,      // No special usage
                CustomVertex.PositionColoredTextured.Format,
                Pool.Managed);

            // Fill the vertex buffer with the corners of a rectangle that covers
            // the entire playing surface.
            GraphicsStream stm = backgroundV.Lock(0, 0, 0);     // Lock the background vertex list
            int clr = Color.Transparent.ToArgb();
            stm.Write(new CustomVertex.PositionColoredTextured(0, 0, 0, clr, 0, 1));
            stm.Write(new CustomVertex.PositionColoredTextured(0, height, 0, clr, 0, 0));
            stm.Write(new CustomVertex.PositionColoredTextured(width, height, 0, clr, 1, 0));
            stm.Write(new CustomVertex.PositionColoredTextured(width, 0, 0, clr, 1, 1));

            backgroundV.Unlock();
        }

        /// <summary>
        /// Render the background image
        /// </summary>
        public void Render()
        {
            device.SetTexture(0, backgroundT);
            device.SetStreamSource(0, backgroundV, 0);
            device.VertexFormat = CustomVertex.PositionColoredTextured.Format;
            device.DrawPrimitives(PrimitiveType.TriangleFan, 0, 2);
            device.SetTexture(0, null);
        }

    }
}