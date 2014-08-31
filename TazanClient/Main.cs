using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace TazanClient
{
    public partial class Main : Form
    {
        public Dictionary<Keys, EDirection> KeyCodeToDirection = new Dictionary<Keys, EDirection>();
        public Main()
        {
            InitializeComponent();

            KeyCodeToDirection[Keys.Down] = EDirection.Down;
        }

        private void Main_Load(object sender, EventArgs e)
        {
            Tazan.inst.InitializeTazan(panel_Render);
            Tazan.inst.InitializeClients(this);
            renderTimer.Enabled = true;
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

        // 클릭 시
        private void panel_Render_MouseClick(object sender, MouseEventArgs e)
        {
            selectedTile.X = e.X / 20;
            selectedTile.Y = e.Y / 20;
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                for (int i = 0; i < Tazan.inst.clients_count; ++i)
                {
                    if (Tazan.inst.clients[i].userInfo.X == selectedTile.X && Tazan.inst.clients[i].userInfo.Y == selectedTile.Y)
                    {
                        Tazan.inst.index = i;
                        textBox_ControledUserID.Text = Tazan.inst.clients[Tazan.inst.index].userInfo.UserID.ToString();
                        textBox_ControledPos.Text = Tazan.inst.clients[Tazan.inst.index].userInfo.X.ToString();
                        textBox_ControledPos.Text += ", ";
                        textBox_ControledPos.Text += Tazan.inst.clients[Tazan.inst.index].userInfo.Y.ToString();
                        textBox_ControledDirection.Text = Tazan.inst.clients[Tazan.inst.index].userInfo.Direction.ToString();
                        break;
                    }
                }
            }
            else if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                if (Tazan.inst.clients[Tazan.inst.index].userInfo.X == selectedTile.X &&
                    Tazan.inst.clients[Tazan.inst.index].userInfo.Y + 1 == selectedTile.Y)
                {
                    BinaryWriter packetWriter = new BinaryWriter(new MemoryStream());
                    packetWriter.Write((short)4);
                    packetWriter.Write((byte)20);
                    packetWriter.Write((byte)0x55);
                    packetWriter.Write((int)EDirection.Down);
                    packetWriter.BaseStream.Seek(0, SeekOrigin.Begin);
                    BinaryReader packetReader = new BinaryReader(packetWriter.BaseStream);
                    byte[] packet = packetReader.ReadBytes(8);
                    Tazan.inst.clients[Tazan.inst.index].StartSend(Tazan.inst.clients[Tazan.inst.index].clientObject, packet);
                }
                else if (Tazan.inst.clients[Tazan.inst.index].userInfo.X - 1 == selectedTile.X &&
                    Tazan.inst.clients[Tazan.inst.index].userInfo.Y == selectedTile.Y)
                {
                    BinaryWriter packetWriter = new BinaryWriter(new MemoryStream());
                    packetWriter.Write((short)4);
                    packetWriter.Write((byte)20);
                    packetWriter.Write((byte)0x55);
                    packetWriter.Write((int)EDirection.Left);
                    packetWriter.BaseStream.Seek(0, SeekOrigin.Begin);
                    BinaryReader packetReader = new BinaryReader(packetWriter.BaseStream);
                    byte[] packet = packetReader.ReadBytes(8);
                    Tazan.inst.clients[Tazan.inst.index].StartSend(Tazan.inst.clients[Tazan.inst.index].clientObject, packet);
                }
                else if (Tazan.inst.clients[Tazan.inst.index].userInfo.X == selectedTile.X &&
                    Tazan.inst.clients[Tazan.inst.index].userInfo.Y - 1 == selectedTile.Y)
                {
                    BinaryWriter packetWriter = new BinaryWriter(new MemoryStream());
                    packetWriter.Write((short)4);
                    packetWriter.Write((byte)20);
                    packetWriter.Write((byte)0x55);
                    packetWriter.Write((int)EDirection.Up);
                    packetWriter.BaseStream.Seek(0, SeekOrigin.Begin);
                    BinaryReader packetReader = new BinaryReader(packetWriter.BaseStream);
                    byte[] packet = packetReader.ReadBytes(8);
                    Tazan.inst.clients[Tazan.inst.index].StartSend(Tazan.inst.clients[Tazan.inst.index].clientObject, packet);
                }
                else if (Tazan.inst.clients[Tazan.inst.index].userInfo.X + 1 == selectedTile.X &&
                    Tazan.inst.clients[Tazan.inst.index].userInfo.Y == selectedTile.Y)
                {
                    BinaryWriter packetWriter = new BinaryWriter(new MemoryStream());
                    packetWriter.Write((short)4);
                    packetWriter.Write((byte)20);
                    packetWriter.Write((byte)0x55);
                    packetWriter.Write((int)EDirection.Right);
                    packetWriter.BaseStream.Seek(0, SeekOrigin.Begin);
                    BinaryReader packetReader = new BinaryReader(packetWriter.BaseStream);
                    byte[] packet = packetReader.ReadBytes(8);
                    Tazan.inst.clients[Tazan.inst.index].StartSend(Tazan.inst.clients[Tazan.inst.index].clientObject, packet);
                }
            }
        }

        private void button_Stop_Click(object sender, EventArgs e)
        {
        }
    }
}
