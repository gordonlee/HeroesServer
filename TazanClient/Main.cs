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
        public Main()
        {
            InitializeComponent();
        }

        private void Main_Load(object sender, EventArgs e)
        {
            Tazan.inst.InitializeTazan(panel_Render);
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

        private void renderTimer_Tick(object sender, EventArgs e)
        {
            Tazan.inst.Update(new Point(selectedTile.X, selectedTile.Y));
        }

        private void button_Stop_Click(object sender, EventArgs e)
        {
            Tazan.inst.index++;
            Tazan.inst.index %= Tazan.clients_count;
            Tazan.inst.InitializeClients();
        }
    }
}
