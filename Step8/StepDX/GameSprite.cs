using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using System.Diagnostics;


namespace StepDX
{
    public class GameSprite : PolygonTextured
    {
        private Vector2 p = new Vector2(0, 0);  // Position
        private Vector2 v = new Vector2(0, 0);  // Velocity
        private Vector2 a = new Vector2(0, 0);  // Acceleration

        public Vector2 P { set { p = value; } get { return p; } }
        public Vector2 V { set { v = value; } get { return v; } }
        public Vector2 A { set { a = value; } get { return a; } }

        private Vector2 pSave;  // Position
        private Vector2 vSave;  // Velocity
        private Vector2 aSave;  // Acceleration


        private float spriteTime = 0;
        private float spriteRate = 6;   // 6 per second

        private bool standing = false;
        public bool Standing { set { standing = value; }  get { return standing; } }


        public void SaveState()
        {
            pSave = p;
            vSave = v;
            aSave = a;
        }

        public void RestoreState()
        {
            p = pSave;
            v = vSave;
            a = aSave;
        }

        protected List<Vector2> verticesM = new List<Vector2>();  // The vertices

        public override List<Vector2> Vertices { get { return verticesM; } }

        public override void Advance(float dt)
        {
            // Euler steps
            v.X += a.X * dt;
            v.Y += a.Y * dt;
            p.X += v.X * dt;
            p.Y += v.Y * dt;


            ////.................................
            if (v.Y != 0 && a.Y != 0)
            {
                standing = false;
            }
            else
                standing = true;    

            ////..................................

            int spriteNum;

            if (v.X == 0)
            {
                spriteNum = 5;
                spriteTime = 0;
            }
            else
            {
                spriteTime += dt;
                spriteNum = (int)(spriteTime * spriteRate) % 4;     // 4 images
            }


            if (v.Y != 0)
            {
                spriteNum = 7;

            }
            if (v.Y <= 0 && !standing)// clear falling flickers
                spriteNum = 5;


            // Create the texture vertices
            textureC.Clear();
            if (v.X >= 0)
            {
                textureC.Add(new Vector2(spriteNum * 0.125f, 1));
                textureC.Add(new Vector2(spriteNum * 0.125f, 0));
                textureC.Add(new Vector2((spriteNum + 1) * 0.125f, 0));
                textureC.Add(new Vector2((spriteNum + 1) * 0.125f, 1));
            }
            else
            {
                // If moving in the negative direction, we draw our sprite 
                // as a mirror image.
                    textureC.Add(new Vector2((spriteNum + 1) * 0.125f, 1));
                    textureC.Add(new Vector2((spriteNum + 1) * 0.125f, 0));
                    textureC.Add(new Vector2(spriteNum * 0.125f, 0));
                    textureC.Add(new Vector2(spriteNum * 0.125f, 1));


            }


            // Move the vertices
            verticesM.Clear();
            foreach (Vector2 x in verticesB)
            {
                verticesM.Add(new Vector2(x.X + p.X, x.Y + p.Y));
            }

            ////Left and Right Bounds
            float playerMinX = 0.4f;                    // Minimum x allowed
            float playerMaxX = 31.6f;                   // Maximum x allowed

            if (p.X < playerMinX)
                p.X = playerMinX;
            else if (p.X > playerMaxX)
                p.X = playerMaxX;





        }



    }
}
