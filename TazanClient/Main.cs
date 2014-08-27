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

namespace TazanClient
{
    public partial class Main : Form
    {
        private Device directxDevice = null;
        private Sprite mainSprite = null;

        private Texture textureTile = null;
        private Texture textureOveredTile = null;
        private Texture textureCharacter = null;

        public Main()
        {
            InitializeComponent();
        }

        private void Main_Load(object sender, EventArgs e)
        {
            // Setting DirectX Device
            PresentParameters pp = new PresentParameters();
            pp.Windowed = true;
            pp.SwapEffect = SwapEffect.Discard;

            directxDevice = new Device(0,
                                DeviceType.Hardware,
                                this.panel_Render,
                                CreateFlags.SoftwareVertexProcessing,
                                pp);

            mainSprite = new Sprite(directxDevice);

            // Texture Load
            textureTile = new Texture(directxDevice, new Bitmap("Resource/tile.png"), Usage.None, Pool.Managed);
            textureOveredTile = new Texture(directxDevice, new Bitmap("Resource/overed_tile.png"), Usage.None, Pool.Managed);
            textureCharacter = new Texture(directxDevice, new Bitmap("Resource/character.png"), Usage.None, Pool.Managed);
        }

        private void textbox_OnlyNum_KeyPress(object sender, KeyPressEventArgs e)
        {
            OnlyNumberTextBox.TypingOnlyNumber(sender, e, false, false);
        }

        Point selectedTile = new Point(0, 0);
        private void panel_Render_MouseMove(object sender, MouseEventArgs e)
        {
            selectedTile.X = e.X / 20;
            selectedTile.Y = e.Y / 20;
        }

        private void panel_Render_Paint(object sender, PaintEventArgs e)
        {
            directxDevice.Clear(ClearFlags.Target, Color.White, 1.0f, 0);
            directxDevice.BeginScene();

            // Render Start
            mainSprite.Begin(SpriteFlags.AlphaBlend);

            // Draw Tiles
            for (int i = 0; i < 30; ++i)
            {
                for (int j = 0; j < 30; ++j)
                {
                    if (selectedTile.X == i && selectedTile.Y == j)
                    {
                        mainSprite.Draw2D(textureOveredTile, Point.Empty, 0f, new Point(i * 20, j * 20), Color.White);
                    }
                    else
                    {
                        mainSprite.Draw2D(textureTile, Point.Empty, 0f, new Point(i * 20, j * 20), Color.White);
                    }
                }
            }

            // Draw Character
            mainSprite.Draw2D(textureCharacter, new Rectangle(0, 0, 20, 20), new Rectangle(0, 0, 20, 20), new Point(1 * 20 + 1, 1 * 20), Color.White);

            mainSprite.End();
            // Render End

            directxDevice.EndScene();
            directxDevice.Present();
        }

        private void renderTimer_Tick(object sender, EventArgs e)
        {
            panel_Render.Refresh();
        }
    }
}
