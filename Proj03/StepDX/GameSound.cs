using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using Microsoft.DirectX;
using Microsoft.DirectX.DirectSound;

namespace StepDX
{
    class GameSound
    {
        private Device SoundDevice = null;

        private SecondaryBuffer[] crash = new SecondaryBuffer[4];
        //int crashToUse = 0;

        private SecondaryBuffer victory = null;
        private SecondaryBuffer boing = null;
        private SecondaryBuffer can_pop = null;
        private SecondaryBuffer crunch = null;
        private SecondaryBuffer woosh = null;


        public GameSound(Form form)
        {
            SoundDevice = new Device();
            SoundDevice.SetCooperativeLevel(form, CooperativeLevel.Priority);

            Load(ref victory, "../../../victory.wav");
            Load(ref boing, "../../../boing.wav");
            Load(ref can_pop, "../../../metal_crunch.wav");

            Load(ref crunch, "../../../can_pop.wav");
            Load(ref woosh, "../../../woosh.wav");

            //for (int i = 0; i < crash.Length; i++)
            //    Load(ref crash[i], "../../click.wav");
        }

        private void Load(ref SecondaryBuffer buffer, string filename)
        {
            try
            {
                buffer = new SecondaryBuffer(filename, SoundDevice);
            }
            catch (Exception)
            {
                MessageBox.Show("Unable to load " + filename, "Danger, Will Robinson", MessageBoxButtons.OK);
                buffer = null;
            }
        }

        //public void Crash()
        //{
        //    crashToUse = (crashToUse + 1) % crash.Length;

        //    if (crash[crashToUse] == null)
        //        return;

        //    if (!crash[crashToUse].Status.Playing)
        //        crash[crashToUse].Play(0, BufferPlayFlags.Default);
        //}

        public void Victory()
        {
            if (victory == null)
                return;

            if (!victory.Status.Playing)
                victory.Play(0, BufferPlayFlags.Looping);
        }
        //public void Nothing()
        //{
        //    if (nothing == null)
        //        return;

        //}

        public void Boing() { Play(boing); }

        public void Can() { Play(can_pop); }

        public void Crunch() { Play(crunch); }

        public void Woosh() { Play(woosh); }

        private void Play(SecondaryBuffer buffer)
        {
            if (buffer == null)
                return;

            if (!buffer.Status.Playing)
                buffer.Play(0, BufferPlayFlags.Default);
        }
    }
}
