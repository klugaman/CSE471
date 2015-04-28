using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;


namespace StepDX
{
    class Ngon:PolygonTextured
    {
        // Vertex data
        //private List<Vector2> verticesB = new List<Vector2>();
        //private VertexBuffer verticesV = null;




        // The texture
        private Texture texture = null;

        // Object position
        private Vector2 p = new Vector2(0, 0);
        public Vector2 P { set { p = value; } get { return p; } }

        // Create an N sided polygon of a given radius.
        public void DrawNgon(int n, float rad)
        {
            double dangle = 2 * Math.PI / n;
            double angle = dangle / 2;

            for (int i = 0; i < n; i++, angle -= dangle)
            {
                Vector2 v = new Vector2();
                v.X = (float)(rad * Math.Cos(angle));
                v.Y = (float)(rad * Math.Sin(angle));
                Vertices.Add(v);
            }
        }



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

                texture = TextureLoader.FromFile(device, "../../../QuestionCoin.png");
            }

            GraphicsStream gs = verticesV.Lock(0, 0, 0);// Lock the background vertex list
            int clr = System.Drawing.Color.Transparent.ToArgb();   // Don't you love all those color names?

            foreach (Vector2 v in verticesB)
            {
                float tu = 0.5f + v.X / 1.8f;
                float tv = 1 - (v.Y - (-0.8f)) / 1.8f;
                gs.Write(new CustomVertex.PositionColoredTextured(v.X + p.X, v.Y + p.Y, 0, clr, tu, tv));
            }

            verticesV.Unlock();

            if (Transparent)
            {
                device.RenderState.AlphaBlendEnable = true;
                device.RenderState.SourceBlend = Blend.SourceAlpha;
                device.RenderState.DestinationBlend = Blend.InvSourceAlpha;
            }

            device.SetTexture(0, texture);
            device.SetStreamSource(0, verticesV, 0);
            device.VertexFormat = CustomVertex.PositionColoredTextured.Format;
            device.DrawPrimitives(PrimitiveType.TriangleFan, 0, Vertices.Count - 2);
            device.SetTexture(0, null);

            if (Transparent)
                device.RenderState.AlphaBlendEnable = false;
        }
    }
}