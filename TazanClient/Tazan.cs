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
    class Tazan
    {
        public static Tazan inst = new Tazan();

        private DXPanel dxPanel = null;
        private PresentParameters presentParameter = null;
        private Device directxDevice = null;
        private Sprite mainSprite = null;

        private Texture textureTile = null;
        private Texture textureOveredTile = null;
        private Texture textureCharacter = null;

        // clients
        public int index = 0;
        public int clients_count = 400;
        public AsyncTcpClient[] clients = null;

        private Tazan()
        {
        }

        ~Tazan()
        {
        }

        public void InitializeTazan(DXPanel panel)
        {
            dxPanel = panel;

            presentParameter = new PresentParameters();
            presentParameter.Windowed = true;
            presentParameter.SwapEffect = SwapEffect.Discard;

            directxDevice = new Device(0,
                                DeviceType.Hardware,
                                dxPanel,
                                CreateFlags.HardwareVertexProcessing,
                                presentParameter);
            mainSprite = new Sprite(directxDevice);

            LoadResources();
        }

        private void LoadResources()
        {
            textureTile = new Texture(directxDevice, new Bitmap("Resource/tile.png"), Usage.None, Pool.Managed);
            textureOveredTile = new Texture(directxDevice, new Bitmap("Resource/overed_tile.png"), Usage.None, Pool.Managed);
            textureCharacter = new Texture(directxDevice, new Bitmap("Resource/character.png"), Usage.None, Pool.Managed);
        }

        private void OnDirectXDeviceLost()
        {
            int nResult;
            while (!directxDevice.CheckCooperativeLevel(out nResult))
            {
                if ((ResultCode)nResult == ResultCode.DeviceNotReset)
                {
                    directxDevice.Reset(new PresentParameters[] { presentParameter });
                }
            }
        }

        public void Update(Point selectedTile)
        {
            if (directxDevice == null)
            {
                return;
            }

            if (mainSprite == null)
            {
                return;
            }

            try
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
                // 아 하드코딩 한거 고치기 귀찮다..
                // 선택한 캐릭터 중심으로 4방향 강조표시
                mainSprite.Draw2D(textureOveredTile, Point.Empty, 0f, new Point((clients[index].userInfo.X - 1) * 20, clients[index].userInfo.Y * 20), Color.White);
                mainSprite.Draw2D(textureOveredTile, Point.Empty, 0f, new Point(clients[index].userInfo.X * 20, (clients[index].userInfo.Y - 1) * 20), Color.White);
                mainSprite.Draw2D(textureOveredTile, Point.Empty, 0f, new Point((clients[index].userInfo.X + 1) * 20, clients[index].userInfo.Y * 20), Color.White);
                mainSprite.Draw2D(textureOveredTile, Point.Empty, 0f, new Point(clients[index].userInfo.X * 20, (clients[index].userInfo.Y + 1) * 20), Color.White);

                if (index >= 0 && index < clients_count)
                {
                    if (clients[index] != null)
                    {
                        // 아 하드코딩 한거 고치기 귀찮다..
                        // 선택한 캐릭
                        mainSprite.Draw2D(textureCharacter, new Rectangle(0, 20 * clients[index].userInfo.Direction, 20, 20), new Rectangle(0, 0, 20, 20), new Point(clients[index].userInfo.X * 20, clients[index].userInfo.Y * 20), Color.White);

                        // 다른 캐릭
                        if (clients[index].otherUserInfo != null)
                        {
                            int size = clients[index].otherUserInfo.Count;
                            for (int j = 0; j < size; ++j)
                            {
                                if (clients[index].otherUserInfo[j].IsShow == false)
                                    continue;
                                mainSprite.Draw2D(textureCharacter, new Rectangle(0, 20 * clients[index].otherUserInfo[j].Direction, 20, 20), new Rectangle(0, 0, 20, 20), new Point(clients[index].otherUserInfo[j].X * 20, clients[index].otherUserInfo[j].Y * 20), Color.White);
                            }
                        }
                    }
                }

                mainSprite.End();
                // Render End

                directxDevice.EndScene();
                directxDevice.Present();
            }
            catch (DeviceLostException lostException)
            {
                OnDirectXDeviceLost();
            }
        }



        public void InitializeClients(Main mainForm)
        {
            clients = new AsyncTcpClient[clients_count];
            for (int i = 0; i < clients_count; ++i)
            {
                clients[i] = new AsyncTcpClient();
                clients[i].Start(mainForm);
            }
        }
    }
}
