using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;


namespace StepDX
{
    class Weird : PolygonTextured
    {
        /// <summary>
        /// The texture map we use for this polygon
        /// </summary>
        private Texture texture = null;

        // Object position
        private Vector2 p = new Vector2(0, 0);
        public Vector2 P { set { p = value; } get { return p; } }


        /// <summary>
        /// The texture map we use for this polygon
        /// </summary>
        public Texture Tex { get { return texture; } set { texture = value; } }
        /// <summary>
        /// List of texture coordinates
        /// </summary>
        protected List<Vector2> textureC = new List<Vector2>();
        // <summary>
        /// Indicates if the texture is transparent
        /// </summary>
        private bool transparent = false;

        /// <summary>
        /// Indicates if the texture is transparent
        /// </summary>
        public bool Transparent { set { transparent = value; } get { return transparent; } }
        /// <summary>
        /// Add a texture coordinate
        /// </summary>
        /// <param name="v">Texture coordinate</param>
        public void AddTex(Vector2 v)
        {
            verticesB.Add(v);
        }
        /// <summary>
        /// Render the textured polygon
        /// </summary>
        /// <param name="device">Device to render onto</param>
        public override void Render(Device device)
        {
            if (verticesV == null)
            {
                verticesV = new VertexBuffer(typeof(CustomVertex.PositionColoredTextured),    // Type of vertex
                   verticesB.Count,      // How many
                   device, // What device
                   0,      // No special usage
                   CustomVertex.PositionColoredTextured.Format,
                   Pool.Managed);


            }

            GraphicsStream gs = verticesV.Lock(0, 0, 0);// Lock the background vertex list
            int clr = System.Drawing.Color.Transparent.ToArgb();   // Don't you love all those color names?

            foreach (Vector2 v in verticesB)
            {
                float tu = 0.5f + v.X / 1.8f;
                float tv = 1 - (v.Y - (-0.8f)) / 1.8f;
                gs.Write(new CustomVertex.PositionColoredTextured(v.X , v.Y , 0, clr, tu, tv));
            }

            verticesV.Unlock();

            device.SetTexture(0, texture);
            device.SetStreamSource(0, verticesV, 0);
            device.VertexFormat = CustomVertex.PositionColoredTextured.Format;

            // Head, one triangle
            device.DrawPrimitives(PrimitiveType.TriangleList, 0, 1);

            // Neck, two triangles
            device.DrawPrimitives(PrimitiveType.TriangleFan, 3, 2);

            // Body, two triangles
            device.DrawPrimitives(PrimitiveType.TriangleFan, 7, 2);

            // Legs
            device.DrawPrimitives(PrimitiveType.TriangleFan, 11, 2);
            device.DrawPrimitives(PrimitiveType.TriangleFan, 15, 2);

            device.SetTexture(0, null);

        }

    }
    
}