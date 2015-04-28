/*! \brief Simple collision checking system.  Only does pairwise collision testing.
 * Assumptions: 
 * Namespace is StepDX.  Please change to suit your project.
 * Existence of a Polygon class with a member function Vertices that returns a
 * List containing Vector2 objects, each the coordinates of a vertex in 
 * clockwise order.
 * \author Charles B. Owen
 * \version 1.00 01-18-2006 Initial standalone version.
 * \version 1.01 11-25-2012 Documentation revisions
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace StepDX
{
    class Collision
    {
        /// <summary>
        /// First polygon collision test result (one with the vertex)
        /// </summary>
        private Polygon poly1 = null;

        /// <summary>
        /// Second polygon collision test result (one with the edge)
        /// </summary>
        Polygon poly2 = null; 

        /// <summary>
        /// The normal at the intersection
        /// </summary>
        Vector2 n = new Vector2();

        /// <summary>
        /// Intersection depth
        /// </summary>
        float depth = 0;

        /// <summary>
        /// True if polygon 1 has the vertex inside polygon 2 edge
        /// </summary>
        bool p1inp2 = false;

        /// <summary>
        /// Intersection depth
        /// </summary>
        public float Depth { get { return depth; } }

        /// <summary>
        /// The normal at the intersection
        /// </summary>
        public Vector2 N { get { return n; } }

        /// <summary>
        /// True if polygon 1 has the vertex inside polygon 2 edge
        /// </summary>
        public bool P1inP2 { get { return p1inp2; } }

        /// <summary>
        /// Performs a collision test for two polygons.
        /// </summary>
        /// <param name="p1">Polygon 1</param>
        /// <param name="p2">Polygon 2</param>
        /// <returns>true if a collision</returns>
        public bool Test(Polygon p1, Polygon p2)
        {
            poly1 = null;
            poly2 = null;
            depth = 0;

            // Symmetrical search for an edge that segments the two...
            if (!TestLR(p1, p2))
                return false;

            if (!TestLR(p2, p1))
                return false;

            // We know we have a collision.  Determine the vertex and edge that 
            // are involved.
            p1inp2 = true;
            if (!TestVE(p1, p2))
            {
                if(!TestVE(p2, p1))
                    return false;

                p1inp2 = false;
            }

            if (poly1 == null || depth == 0)
            {
                // No actual collision...
                return false;
            }

            return true;
        }

        // Test of all vertices in c1 against all edges in c2.
        // Returns false if there is an edge that separates the two
        // True simply means potential for an overlap
        private bool TestLR(Polygon p1, Polygon p2)
        {
            List<Vector2> v1 = p1.Vertices;
			List<Vector2> v2 = p2.Vertices;

            // Last vertex in v2
            Vector2 v2a = (Vector2)v2[v2.Count - 1];
            int cnt2 = 0;       // Edge counter

            foreach (Vector2 v2b in v2)
            {
                // Compute the edge line function
                float a = v2a.Y - v2b.Y;
                float b = v2b.X - v2a.X;
                float c = -a * v2a.X - b * v2a.Y;

                bool possible = true;
                int cnt1 = 0;           // Vertex counter
                foreach (Vector2 v1a in v1)
                {
                    float r = a * v1a.X + b * v1a.Y + c;

                    // If r <= zero, we're on the wrong side of the line.
                    // This can't be a separator line.
                    if (r <= 0)
                    {
                        possible = false;
                        break;
                    }

                    cnt1++;
                }

                // If the possibility that this is a separator line never got cleared,
                // it just me one.
                if (possible)
                {
                    // We have a separator line.  This is our witness, save it
                    return false;
                }

                // Make this the end point for the next pass
                v2a = v2b;
                cnt2++;
            }

            // If no separating edges have been found, we potentially have an overlap
            return true;
        }


        /// <summary>
        /// Test of all vertices in p1 against all edges in p2.
        /// This is looking for a vertex in c1 that is inside c2, a 
        /// symmetrical problem to the above tests.
        /// </summary>
        /// <param name="p1">Polygon 1</param>
        /// <param name="p2">Polygon 2</param>
        /// <returns>true if the collision point was found.</returns>
        private bool TestVE(Polygon p1, Polygon p2)
        {
			List<Vector2> v1 = p1.Vertices;
			List<Vector2> v2 = p2.Vertices;

            // 
            // Loop over all of the vertices.
            // We are looking for the one vertex that 
            // is most deeply embedded in the object.  
            //

            bool anyVert = false;                   // Until we know
            float bestVertR = 0.0f;                 // Looking for deepest, so start low
            Vector2 bestVertN = new Vector2();      // Best intersection normal

            foreach (Vector2 v1a in v1)
            {
                bool possible = true;               // Candidate vertex

                //
                // Loop over all of the edges. Is this vertex contained by all?
                //

                // Last vertex in v2
                Vector2 v2a = (Vector2)v2[v2.Count - 1];

                float bestR = 1e10f;                // Least penetration for this vertex
                Vector2 bestN = new Vector2();      // Best intersection normal

                foreach (Vector2 v2b in v2)
                {
                    // Compute the edge line function
                    float a = v2a.Y - v2b.Y;
                    float b = v2b.X - v2a.X;

                    // Normalize these values
                    float len = (float)Math.Sqrt(a * a + b * b);
                    a /= len;
                    b /= len;

                    // Compute c and r...
                    float c = -a * v2a.X - b * v2a.Y;
                    float r = a * v1a.X + b * v1a.Y + c;

                    // If r <= zero, we're on the inside side of the line.
                    if (r > 0)
                    {
                        possible = false;
                        break;
                    }
                    else if (-r < bestR)
                    {
                        bestR = -r;
                        bestN.X = a;
                        bestN.Y = b;
                    }

                    // Make this the end point for the next pass
                    v2a = v2b;
                }

                // The possibility this vertex is inside never got cleared.  
                // It must be actually inside.
                if (possible && bestR > bestVertR)
                {
                    bestVertN = bestN;
                    bestVertR = bestR;
                    anyVert = true;
                }

            }

            if (anyVert)
            {
                poly1 = p1;
                poly2 = p2;
                n = bestVertN;
                depth = bestVertR;
                return true;
            }

            // If no penetrating vertex is found.
            return false;
        }
    }
}

