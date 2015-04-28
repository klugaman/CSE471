using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace StepDX
{
    public class Polygon
    {
        //
        // The vertices of the polygon
        //

        /// <summary>
        /// The raw vertex values we are supplied
        /// </summary>
        protected List<Vector2> verticesB = new List<Vector2>();

        /// <summary>
        /// Vertex buffer used for drawing
        /// </summary>
        protected VertexBuffer verticesV = null;

        /// <summary>
        /// The color we will draw this polygon
        /// </summary>
        protected Color color = Color.AntiqueWhite;

        /// <summary>
        /// A property: The color we will draw the polygon
        /// </summary>
        public Color Color { set { color = value; } get { return color; } }

        /// <summary>
        /// Access to the list of vertices
        /// </summary>
        public virtual List<Vector2> Vertices { get { return verticesB; } }


        /// <summary>
        /// Add a vertex to the polygon.  Must be called before the
        /// first rendering of the polygon.
        /// </summary>
        /// <param name="vertex">The vertex to add</param>
        public void AddVertex(Vector2 vertex)
        {
            if (verticesV == null)
                verticesB.Add(vertex);
        }


        /// <summary>
        /// Draw the polygon
        /// </summary>
        /// <param name="device">The device to draw on</param>
        public virtual void Render(Device device)
        {
            // Get the vertices we will draw
            List<Vector2> vertices = Vertices;

            // Ensure we have at least a triangle
            if (vertices.Count < 3)
                return;

            if (verticesV == null)
            {
                verticesV = new VertexBuffer(typeof(CustomVertex.PositionColored),    // Type
                   vertices.Count,      // How many
                   device, // What device
                   0,      // No special usage
                   CustomVertex.PositionColored.Format,
                   Pool.Managed);
            }

            GraphicsStream gs = verticesV.Lock(0, 0, 0);     // Lock the background vertex list
            int clr = color.ToArgb();

            foreach (Vector2 v in vertices)
            {
                gs.Write(new CustomVertex.PositionColored(v.X, v.Y, 0, clr));
            }

            verticesV.Unlock();

            device.SetStreamSource(0, verticesV, 0);
            device.VertexFormat = CustomVertex.PositionColored.Format;
            device.DrawPrimitives(PrimitiveType.TriangleFan, 0, vertices.Count - 2);
        }

        /// <summary>
        /// Support for advancing an animation
        /// </summary>
        /// <param name="dt">A delta time amount in seconds</param>
        public virtual void Advance(float dt)
        {
        }

    }
}