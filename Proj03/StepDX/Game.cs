using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
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
        /// Polygon floor
        /// </summary>
       // Polygon floor = new Polygon();
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

        GameSound sounds = null;

        float prev_player_loc_y = 0;
        float prev_player_loc_x = 0;

        /// <summary>
        /// /////////////////////////////////////
        /// </summary>
        Random random = new Random();

        private int score = 0;

        private Microsoft.DirectX.Direct3D.Font font;

        List<Ngon> coins = new List<Ngon>();




        public Game()
        {
            InitializeComponent();
            if (!InitializeDirect3D())
                return;

            sounds = new GameSound(this);
            sounds.Victory();

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



            // To create a rectangle
            vertices = new VertexBuffer(typeof(CustomVertex.PositionColored), // Type of vertex
                                       4,      // How many
                                       device, // What device
                                       0,      // No special usage
                                       CustomVertex.PositionColored.Format,
                                       Pool.Managed);
            background = new Background(device, playingW, playingH);


            //draw floor /////////////////////////////////////////////////////////////
            Texture floor = TextureLoader.FromFile(device, "../../../floor.jpg");

            AddTexture(0, playingW / 3, 0, 1, Color.Transparent, floor, 0, 1, 0, 1);

            Texture poolTexture = TextureLoader.FromFile(device, "../../../pool.png");
            AddBadPoly(playingW / 3, playingW / 3 * (1 + 0.5f), 0, 0.7f, Color.Transparent, poolTexture, 0, 1, 0, 1);

            AddTexture(playingW / 3 * (1 + 0.5f), playingW, 0, 1, Color.Transparent, floor, 0, 1, 0, 1);


            //create a platform
            AddPlatform(3.2f, 3.9f, 1.8f, 2, Color.CornflowerBlue);

            //create a platform
            AddPlatform2(playingW / 3, playingW / 3 + 0.7f, 0.8f, 1, Color.Black);

            AddCoins();


            Texture c1 = TextureLoader.FromFile(device, "../../../candy1.png");
            Texture c2 = TextureLoader.FromFile(device, "../../../candy2.png");
            AddBadPoly(13.1f, 13.9f, 3.2f, 3.4f, Color.Transparent, c1, 0.3f, 1, 0.55f, 0.85f);

            AddBadPoly(7.1f, 8.1f, 2.2f, 2.8f, Color.Transparent, c2, 0.2f, 1, 0.05f, 0.45f);

            AddBadPoly(26, 27, 1.5f, 2.1f, Color.Transparent, c2, 0.2f, 1, 0.05f, 0.45f);

            AddObstacle(27.1f, 27.7f, 3.0f, 3.2f, Color.BlanchedAlmond);

            /////////////////////////////////////////////////////////////////////////////////////////////////////





            ////Game
            AddObstacle(4, 4.2f, 1, 2.1f, Color.Coral);
            AddObstacle(5, 6, 1.4f, 1.6f, Color.BurlyWood);
            AddObstacle(5.5f, 6.5f, 3.2f, 3.4f, Color.PeachPuff);
            AddObstacle(8.5f, 9.5f, 2.5f, 2.7f, Color.Brown);


            AddObstacle(10.5f, 11.5f, 1.5f, 1.7f, Color.AliceBlue);

            ///add

            AddObstacle(11.5f, 12.5f, 2.2f, 2.4f, Color.PeachPuff);
            // AddObstacle(13.5f, 15f, 3.2f, 3.4f, Color.BlanchedAlmond);
            AddObstacle(14.5f, 15.5f, 2f, 2.2f, Color.Aqua);

            AddObstacle(22.5f, 24f, 3.0f, 3.2f, Color.Crimson);//red
            AddObstacle(29.1f, 29.9f, 2.5f, 2.8f, Color.Chocolate);
        


            Platform platform = new Platform();
            platform.AddVertex(new Vector2(3.2f, 2));
            platform.AddVertex(new Vector2(3.9f, 2));
            platform.AddVertex(new Vector2(3.9f, 1.8f));
            platform.AddVertex(new Vector2(3.2f, 1.8f));
            platform.Color = Color.Black;
            world.Add(platform);

        
            ///OBJECT1
            Texture weird_texture = TextureLoader.FromFile(device, "../../../green.jpg");
            Weird wd = new Weird();
            wd.Tex = weird_texture;
            //triangle  offset=0
            wd.AddVertex(new Vector2(1.5f, 1.1f));
            wd.AddVertex(new Vector2(1f, 1.5f));
            wd.AddVertex(new Vector2(2f, 1.5f));
            world.Add(wd);


              

            ///OBJECT2
            Texture texture = TextureLoader.FromFile(device, "../../../stone08.bmp");
            PolygonTextured pt = new PolygonTextured();
            pt.Tex = texture;
            pt.AddVertex(new Vector2(17.5f, 1f));
            pt.AddTex(new Vector2(0, 0));
            pt.AddVertex(new Vector2(17f, 1.5f));
            pt.AddTex(new Vector2(0, 1));

            pt.AddVertex(new Vector2(19f, 1.5f));
            pt.AddTex(new Vector2(1, 1));

            pt.AddVertex(new Vector2(18.5f, 1f));
            pt.AddTex(new Vector2(1, 0));

 

            pt.Color = Color.Transparent;
            world.Add(pt);


            ///OBJECT3
            Texture texture3 = TextureLoader.FromFile(device, "../../../brown.jpg");
            PolygonTextured pt3 = new PolygonTextured();
            pt3.Tex = texture3;
            pt3.AddVertex(new Vector2(20, 2.7f)); //A
            pt3.AddTex(new Vector2(0, 1));
            pt3.AddVertex(new Vector2(21, 2.7f)); //B
            pt3.AddTex(new Vector2(0.5f, 0));
            pt3.AddVertex(new Vector2(21, 2.5f)); //C
            pt3.AddTex(new Vector2(1, 0.5f));
            pt3.AddVertex(new Vector2((20 + 21) / 2.0f, 2)); //D
            pt3.AddTex(new Vector2(1, 1));
            pt3.AddVertex(new Vector2(20, 2.5f));
            pt3.AddTex(new Vector2(0, 0.5f));
            world.Add(pt3);



            
            ///Object4
            Texture texture4 = TextureLoader.FromFile(device, "../../../green.jpg");
            PolygonTextured pt4 = new PolygonTextured();
            pt4.Tex = texture4;
    

            pt4.AddVertex(new Vector2(5.375f, 1f));
            pt4.AddTex(new Vector2(1, 0.5f));
            pt4.AddVertex(new Vector2(5f, 1.2f));
            pt4.AddTex(new Vector2(0, 0));
            pt4.AddVertex(new Vector2(5.5f, 1.4f));
            pt4.AddTex(new Vector2(0, 0.5f));
            pt4.AddVertex(new Vector2(5.5f, 1.2f));
            pt4.AddTex(new Vector2(0.5f, 0));
            pt4.AddVertex(new Vector2(6f, 1.2f));
            pt4.AddTex(new Vector2(0.5f, 0.5f));
            pt4.AddVertex(new Vector2(5.625f, 1f));
            pt4.AddTex(new Vector2(0.5f, 1));

  
            world.Add(pt4);
        

            Texture spritetexture = TextureLoader.FromFile(device, "../../../guy8.bmp");
            player.Transparent = true;
            player.P = new Vector2(0.5f, 1);
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
            // Determine the last time
            stopwatch.Start();
            lastTime = stopwatch.ElapsedMilliseconds;
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
            // Static background
            //device.Transform.Projection = Matrix.OrthoOffCenterLH(0, widP, 0, playingH, 0, 1);
            
            // Background moves with player
            //float winCenter = playerLoc.X;
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
            // Render the floor polygon
            //floor.Render(device);
            // Render all the polygons in the world
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

        private void AddObstacle(float p1, float p2, float p3, float p4, Color color)
        {
            Polygon p = new Polygon();
            p.AddVertex(new Vector2(p1, p4));
            p.AddVertex(new Vector2(p2, p4));
            p.AddVertex(new Vector2(p2, p3));
            p.AddVertex(new Vector2(p1, p3));
            p.Color = color;
            world.Add(p);
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


            List<Polygon> dangers = new List<Polygon>();


            while (delta > 0)
            {

                float step = delta;
                if (step > 0.05f)
                    step = 0.05f;

                float maxspeed = Math.Max(Math.Abs(player.V.X), Math.Abs(player.V.Y));
                if (maxspeed > 0)
                {
                    step = (float)Math.Min(step, 0.05 / maxspeed);
                }
               
                player.Advance(step);


                foreach (Polygon p in world)
                {
                    p.Advance(step);
                }
                //Some refactoring

                

                foreach (Polygon p in world)
                {
                   // p.Advance(step);
                    if (collision.Test(player, p))
                    {
                        /////////////////////////////////////////////////////
                          if (p.IsBad)
                             dangers.Add(p);


                        float depth = collision.P1inP2 ?
                                  collision.Depth : -collision.Depth;
                        player.P = player.P + collision.N * depth;
                        Vector2 v = player.V;
                        if (collision.N.X != 0)
                            v.X = 0;
                        if (collision.N.Y != 0)
                            v.Y = 0;
                            
                        player.V = v;


                        if (prev_player_loc_y != player.P.Y && !(p is Platform))
                        {
                            sounds.Can();
                        }

                        if (prev_player_loc_y != player.P.Y && (p is Platform))
                        {
                            sounds.Woosh();
                        }

                        if (prev_player_loc_x != player.P.X)
                        {
                            sounds.Crunch();
                        }

                        player.Advance(0);
                
                    }
                }

                delta -= step;



                /////////////////////////////////////////////////////////////////
                //foreach (Polygon b in dangers)
                //    world.Remove(b);



                List<Ngon> newfireballs = new List<Ngon>();
                foreach (Ngon f in coins)
                {
                    if (Math.Abs(f.P.X - player.P.X) < ((float)(0.2f + 0.2f) / 2 + f.Rad) && Math.Abs(f.P.Y - player.P.Y) < ((float)(1) / 2 + f.Rad))
                    {
                        score += 10;
                        world.Remove(f);
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

            prev_player_loc_y = player.P.Y;
            prev_player_loc_x = player.P.X;

            foreach (Polygon b in dangers)
                world.Remove(b);


            if(player.P.Y<-2)
            {
                world.Clear();
                
                System.Windows.Forms.MessageBox.Show("You Fall into the River. Game Over!");
                

                device = null;
                
                player = null;
                this.Close(); // Esc was pressed
            }


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
                // Task 2: Allow jumping only if player is standing on something
                if (player.OnBlock)
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
            /*if (e.KeyCode == Keys.Right || e.KeyCode == Keys.Left)
                playerSpeed.X = 0;
            */
            if (e.KeyCode == Keys.Right || e.KeyCode == Keys.Left)
            {
                Vector2 v = player.V;
                v.X = 0;
                player.V = v;
            }
        }

        private void Game_Load(object sender, EventArgs e)
        {

        }

        /////////////////////////////////////////////////////////

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

        public void AddPlatform2(float left, float right, float bottom, float top, Color color)
        {

            Platform2 platform = new Platform2();
            platform.AddVertex(new Vector2(left, top));
            platform.AddVertex(new Vector2(right, top));
            platform.AddVertex(new Vector2(right, bottom));
            platform.AddVertex(new Vector2(left, bottom));
            platform.Color = color;
            world.Add(platform);

        }


        public void AddTexture(float left, float right, float bottom, float top, Color color, Texture texture, float left_texture, float right_texture, float bottom_texture, float top_texture)
        {
            PolygonTextured pool = new PolygonTextured();
            pool.Tex = texture;
            pool.AddVertex(new Vector2(left, bottom));
            pool.AddTex(new Vector2(left_texture, top_texture));
            pool.AddVertex(new Vector2(left, top));
            pool.AddTex(new Vector2(left_texture, bottom_texture));
            pool.AddVertex(new Vector2(right, top));
            pool.AddTex(new Vector2(right_texture, bottom_texture));
            pool.AddVertex(new Vector2(right, bottom));
            pool.AddTex(new Vector2(right_texture, top_texture));
            pool.Color = color;
            pool.Transparent = true;
            world.Add(pool);

        }

        public void AddCoins()
        {

            // Create cnt * cnt polygons
            int cnt = 6;
            float rad = 0.2f;
            Texture texN = TextureLoader.FromFile(device, "../../../QuestionCoin.png");

            for (int i = 0; i < cnt; i++)
            {

                float x = (float)((0.5f) + (playingW - rad * (cnt + 1)) / cnt * i);
                float y = (float)(random.Next(1, 3) * (playingH - 2f) / 3 + 1.2f);


                for (int j = 1; j <= cnt; j++)
                {
                    Ngon p = new Ngon();

                    p.Rad = 0.2f;
                    p.DrawNgon(10, p.Rad);
                    p.Tex = texN;



                    if (x + 0.5f * j < playingW - 0.4f)
                    {
                        p.P = new Vector2((float)(x + 0.5f * j),
                         (float)(y));
                        coins.Add(p);

                        world.Add(p);


                    }

                }
            }

        }//end AddCoins


        public void AddBadPoly(float left, float right, float bottom, float top, Color color, Texture texture, float left_texture, float right_texture, float bottom_texture, float top_texture)
        {
            PolygonTextured pool = new PolygonTextured();
            pool.Tex = texture;
            pool.AddVertex(new Vector2(left, bottom));
            pool.AddTex(new Vector2(left_texture, top_texture));
            pool.AddVertex(new Vector2(left, top));
            pool.AddTex(new Vector2(left_texture, bottom_texture));
            pool.AddVertex(new Vector2(right, top));
            pool.AddTex(new Vector2(right_texture, bottom_texture));
            pool.AddVertex(new Vector2(right, bottom));
            pool.AddTex(new Vector2(right_texture, top_texture));
            pool.Color = color;
            pool.Transparent = true;
            pool.IsBad = true;
            world.Add(pool);

        }






    }
}
