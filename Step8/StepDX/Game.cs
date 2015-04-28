using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;



namespace StepDX
{
    public partial class Game : Form
    {
        /// <summary>
        /// The DirectX device we will draw on
        /// </summary>
        private Device device = null;

        /// <summary>
        /// Height of our playing area (meters)
        /// </summary>
        private float playingH = 4;

        /// <summary>
        /// Width of our playing area (meters)
        /// </summary>
        private float playingW = 32;

        /// <summary>
        /// Vertex buffer for our drawing
        /// </summary>
        private VertexBuffer vertices = null;

        /// <summary>
        /// The background image class
        /// </summary>
        private Background background = null;

       

        /// <summary>
        /// What the last time reading was
        /// </summary>
        private long lastTime;

        /// <summary>
        /// A stopwatch to use to keep track of time
        /// </summary>
        private System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();

        /// <summary>
        /// All of the polygons that make up our world
        /// </summary>
        List<Polygon> world = new List<Polygon>();

        /// <summary>
        /// Our player sprite
        /// </summary>
        GameSprite player = new GameSprite();

        /// <summary>
        /// The collision testing subsystem
        /// </summary>
        Collision collision = new Collision();

        Random random = new Random();

        private int score = 0;

        private Microsoft.DirectX.Direct3D.Font font;

        List<Ngon> coins = new List<Ngon>();


        //constructor
        public Game()
        {
            InitializeComponent();

            if (!InitializeDirect3D())
                return;

            font = new Microsoft.DirectX.Direct3D.Font(device,  // Device we are drawing on
                20,         // Font height in pixels
                0,          // Font width in pixels or zero to match height
                FontWeight.Bold,    // Font weight (Normal, Bold, etc.)
                0,          // mip levels (0 for default)
                false,      // italics?
                CharacterSet.Default,   // Character set to use
                Precision.Default,      // The font precision, try some of them...
                FontQuality.Default,    // Quality?
                PitchAndFamily.FamilyDoNotCare,     // Pitch and family, we don't care
                "Arial");               // And the name of the font


            vertices = new VertexBuffer(typeof(CustomVertex.PositionColored), // Type of vertex
                                        4,      // How many
                                        device, // What device
                                        0,      // No special usage
                                        CustomVertex.PositionColored.Format,
                                        Pool.Managed);

            background = new Background(device, playingW, playingH);
            // Determine the last time
            stopwatch.Start();
            lastTime = stopwatch.ElapsedMilliseconds;


            Polygon floor = new Polygon();
            floor.AddVertex(new Vector2(0, 1));
            floor.AddVertex(new Vector2(playingW, 1));
            floor.AddVertex(new Vector2(playingW, 0.9f));
            floor.AddVertex(new Vector2(0, 0.9f));
            floor.Color = Color.CornflowerBlue;
            world.Add(floor);

            AddObstacle(2, 3, 1.7f, 1.9f, Color.Crimson);
            AddObstacle(4, 4.2f, 1, 2.1f, Color.Coral);
            AddObstacle(5, 6, 2.2f, 2.4f, Color.BurlyWood);
            AddObstacle(5.5f, 6.5f, 3.2f, 3.4f, Color.PeachPuff);
            AddObstacle(6.5f, 7.5f, 2.5f, 2.7f, Color.Chocolate);


            //create a platform
            AddPlatform(3.2f, 3.9f, 1.8f, 2, Color.CornflowerBlue);


            //load the texture and create an object
            Texture texture = TextureLoader.FromFile(device, "../../../stone08.bmp");
            AddTexture(1.2f, 1.9f, 3.3f, 3.5f, Color.Transparent, texture, 0, 1, 0, 1);

            Texture spritetexture = TextureLoader.FromFile(device, "../../../guy8.bmp");
            player.Tex = spritetexture;
            player.AddVertex(new Vector2(-0.2f, 0));
            player.AddTex(new Vector2(0, 1));
            player.AddVertex(new Vector2(-0.2f, 1));
            player.AddTex(new Vector2(0, 0));
            player.AddVertex(new Vector2(0.2f, 1));
            player.AddTex(new Vector2(0.125f, 0));
            player.AddVertex(new Vector2(0.2f, 0));
            player.AddTex(new Vector2(0.125f, 1));
            player.Color = Color.Transparent;
            player.Transparent = true;
            player.P = new Vector2(0.5f, 1);

            AddNgon();

        }

        public void Render()
        {
            if (device == null)
                return;

            device.Clear(ClearFlags.Target, System.Drawing.Color.Blue, 1.0f, 0);

            int wid = Width;                            // Width of our display window
            int hit = Height;                           // Height of our display window.
            float aspect = (float)wid / (float)hit;     // What is the aspect ratio?

            device.RenderState.ZBufferEnable = false;   // We'll not use this feature
            device.RenderState.Lighting = false;        // Or this one...
            device.RenderState.CullMode = Cull.None;    // Or this one...

            float widP = playingH * aspect;         // Total width of window
            // location of window
            float winCenter = player.P.X;
            if (winCenter - widP / 2 < 0)
                winCenter = widP / 2;
            else if (winCenter + widP / 2 > playingW)
                winCenter = playingW - widP / 2;

            device.Transform.Projection = Matrix.OrthoOffCenterLH(winCenter - widP / 2,
                                                                  winCenter + widP / 2,
                                                                  0, playingH, 0, 1);

            //Begin the scene
            device.BeginScene();

            // Render the background
            background.Render();

            foreach (Polygon p in world)
            {
                p.Render(device);
            }



            player.Render(device);


            font.DrawText(null,     // Because I say so
                      "Score:" + score,            // Text to draw
                      new Point(10, 10),  // Location on the display (pixels with 0,0 as upper left)
                      Color.LightCyan);   // Font color



            //End the scene
            device.EndScene();
            device.Present();
        }

        /// <summary>
        /// Initialize the Direct3D device for rendering
        /// </summary>
        /// <returns>true if successful</returns>
        private bool InitializeDirect3D()
        {
            try
            {
                // Now let's setup our D3D stuff
                PresentParameters presentParams = new PresentParameters();
                presentParams.Windowed = true;
                presentParams.SwapEffect = SwapEffect.Discard;

                device = new Device(0, DeviceType.Hardware, this, CreateFlags.SoftwareVertexProcessing, presentParams);
            }
            catch (DirectXException)
            {
                return false;
            }

            return true;
        }


        protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
                this.Close(); // Esc was pressed
            else if (e.KeyCode == Keys.Right)
            {
                Vector2 v = player.V;
                v.X = 1.5f;
                player.V = v;
            }
            else if (e.KeyCode == Keys.Left)
            {
                Vector2 v = player.V;
                v.X = -1.5f;
                player.V = v;
            }
            else if (e.KeyCode == Keys.Space)
            {
                if (player.Standing)
                {
                    Vector2 v = player.V;
                    v.Y = 7;
                    player.V = v;
                    player.A = new Vector2(0, -9.8f);

                }

            }

        }

        protected override void OnKeyUp(System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Right || e.KeyCode == Keys.Left)
            {
                Vector2 v = player.V;
                v.X = 0;
                player.V = v;
            }
        }

        /// <summary>
        /// Advance the game in time
        /// </summary>
        public void Advance()
        {
            // How much time change has there been?
            long time = stopwatch.ElapsedMilliseconds;
            float delta = (time - lastTime) * 0.001f;       // Delta time in milliseconds
            lastTime = time;

            while (delta > 0)
            {
                // fix tunnelling
                float step = delta;
                if (step > 0.05f)
                    step = 0.05f;

                float maxspeed = Math.Max(Math.Abs(player.V.X), Math.Abs(player.V.Y));
                if (maxspeed > 0)
                {
                    step = (float)Math.Min(step, 0.05 / maxspeed);
                }

                ////
             
                player.Advance(step);
              

                foreach (Polygon p in world)
                {
 
                    p.Advance(step);


                }
                   

                foreach (Polygon p in world)
                {
                    if (collision.Test(player, p))
                    {

                        float depth = collision.P1inP2 ?
                                  collision.Depth : -collision.Depth;
                        player.P = player.P + collision.N * depth;
                        Vector2 v = player.V;
                        
                        if (collision.N.X != 0)
                            v.X = 0;
                        if (collision.N.Y != 0)
                            v.Y = 0;
                        player.V = v;
                        player.Advance(0);
                    }
                }

                delta -= step;

                List<Ngon> newfireballs = new List<Ngon>();
                 foreach (Ngon f in coins)
                 {
                    if (collision.Test(player, f))
                    {
                        score += 10;
                        // Score a collision with p
                        // and we won't need this fireball anymore.
                    }
                    else
                    {
                        // We still need this fireball, save it to the new list
                        newfireballs.Add(f);
                    }

                 }//end foreach

                 coins = newfireballs;



            }

        }

        public void AddObstacle(float left, float right, float bottom, float top, Color color)
        {
            Polygon floor = new Polygon();
            floor.AddVertex(new Vector2(left, top));
            floor.AddVertex(new Vector2(right, top));
            floor.AddVertex(new Vector2(right, bottom));
            floor.AddVertex(new Vector2(left, bottom));
            floor.Color = color;
            world.Add(floor);
        }

        public void AddPlatform(float left, float right, float bottom, float top, Color color)
        {

            Platform platform = new Platform();
            platform.AddVertex(new Vector2(left, top));
            platform.AddVertex(new Vector2(right, top));
            platform.AddVertex(new Vector2(right, bottom));
            platform.AddVertex(new Vector2(left, bottom));
            platform.Color = color;
            world.Add(platform);

        }

        public void AddTexture(float left, float right, float bottom, float top, Color color,Texture texture, float left_texture, float right_texture, float bottom_texture, float top_texture)
        {
            PolygonTextured pt = new PolygonTextured();
            pt.Tex = texture;
            pt.AddVertex(new Vector2(left, top));
            pt.AddTex(new Vector2(left_texture, top_texture));
            pt.AddVertex(new Vector2(right, top));
            pt.AddTex(new Vector2(left_texture, bottom_texture));
            pt.AddVertex(new Vector2(right, bottom));
            pt.AddTex(new Vector2(right_texture, bottom_texture));
            pt.AddVertex(new Vector2(left, bottom));
            pt.AddTex(new Vector2(right_texture, top_texture));
            pt.Color = color;
            world.Add(pt);

        }

        public void AddNgon()
        {
            float worldW = Width - 0.4f;
            float worldH = Height - 0.6f;
            // Create cnt * cnt polygons
            int cnt = 6;
            float rad = 0.5f;

            for (int i = 0; i <cnt; i++)
            {
                
                float x = (float)((0.5f) + (playingW - rad*(cnt+1)) / cnt*i);
                float y = (float)(random.Next(1, 3) * (playingH-2f) / 3 + 1.2f);
               
                for (int j = 1; j <= cnt; j++)
                {
                    Ngon p = new Ngon();
                    p.Color = Color.Transparent;
                    p.DrawNgon(10, rad);

                    if (x + 0.5f * j < playingW-0.4f)
                    {
                        p.P = new Vector2((float)(x + 0.5f * j),
                         (float)(y));
                        coins.Add(p);

                        world.Add(p);

                    }
                    
                }
                }
            }//end AddNgon
               
        

    }
}