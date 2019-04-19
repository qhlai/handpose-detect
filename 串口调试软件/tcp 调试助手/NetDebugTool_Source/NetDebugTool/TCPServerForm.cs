namespace NetDebugTool
{
    using DevComponents.DotNetBar;
    using DevComponents.DotNetBar.Controls;
    using nsoftware.IPWorks;
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Drawing;
    using System.Runtime.CompilerServices;
    using System.Windows.Forms;

    public class TCPServerForm : Office2007Form, ISendUC
    {
        private Bar bar1;
        private ButtonX buttonX_start;
        private ButtonX buttonX_stop;
        private CheckBoxX checkBoxX1;
        private List<int> ClientToSendList;
        private ColumnHeader columnHeader_index;
        private ColumnHeader columnHeader_ip;
        private ColumnHeader columnHeader_port;
        private IContainer components = null;
        private ExpandableSplitter expandableSplitter1;
        private ExpandableSplitter expandableSplitter2;
        private GroupBox groupBox1;
        private GroupBox groupBox2;
        private Dictionary<int, ListViewItem> HS_Innerid_ListviewItem;
        private Ipdaemon ipdaemon1;
        private bool isecho = false;
        private LabelX labelX2;
        private ListViewEx listViewEx1;
        private NumericUpDown numericUpDown1;
        private object ObjectForLock;
        private Panel panel_bottom;
        private Panel panel_left;
        private Panel panel_main;
        private Panel panel1;
        private Panel panel2;
        private ReceiveUc receiveUc1;
        private SendUc sendUc1;

        public event EventHandler ClientToSendChanged;

        public TCPServerForm()
        {
            this.InitializeComponent();
            this.ipdaemon1.OnDataIn += new Ipdaemon.OnDataInHandler(this.ipdaemon1_OnDataIn);
            this.ipdaemon1.OnConnected += new Ipdaemon.OnConnectedHandler(this.ipdaemon1_OnConnected);
            this.ipdaemon1.OnError += new Ipdaemon.OnErrorHandler(this.ipdaemon1_OnError);
            this.ipdaemon1.OnDisconnected += new Ipdaemon.OnDisconnectedHandler(this.ipdaemon1_OnDisconnected);
            this.ClientToSendList = new List<int>();
            this.HS_Innerid_ListviewItem = new Dictionary<int, ListViewItem>();
            this.ObjectForLock = new object();
            this.sendUc1.isend = this;
        }

        private void AddItemToListview(ListViewItem lvi)
        {
            if (this.listViewEx1.InvokeRequired)
            {
                this.listViewEx1.BeginInvoke(new AddItemToListviewCallBack(this.AddItemToListview), new object[] { lvi });
            }
            else
            {
                this.listViewEx1.Items.Add(lvi);
            }
        }

        private void buttonX_start_Click(object sender, EventArgs e)
        {
            try
            {
                this.ipdaemon1.LocalPort = (int) this.numericUpDown1.Value;
                this.ipdaemon1.Listening = true;
                this.buttonX_start.Enabled = false;
                this.buttonX_stop.Enabled = true;
                this.receiveUc1.SetLog(new LogData(string.Format("启动成功,端口号:{0}", this.ipdaemon1.LocalPort), 1));
                this.listViewEx1.Items.Clear();
                this.Text = "TCP Server --- " + this.ipdaemon1.LocalPort;
            }
            catch (Exception ee)
            {
                this.receiveUc1.SetLog(new LogData(string.Format("启动失败:{0}", ee.Message), 3));
            }
        }

        private void buttonX_stop_Click(object sender, EventArgs e)
        {
            try
            {
                this.ipdaemon1.Listening = false;
                this.buttonX_start.Enabled = true;
                this.buttonX_stop.Enabled = false;
                this.receiveUc1.SetLog(new LogData(string.Format("停止成功,端口号:{0}", this.ipdaemon1.LocalPort), 1));
                lock (this.ObjectForLock)
                {
                    this.HS_Innerid_ListviewItem.Clear();
                    this.listViewEx1.Items.Clear();
                }
            }
            catch (Exception ee)
            {
                this.receiveUc1.SetLog(new LogData(string.Format("停止失败:{0}", ee.Message), 3));
            }
        }

        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            this.isecho = this.checkBoxX1.Checked;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        public IList<int> GetClientsForSend()
        {
            return this.ClientToSendList;
        }

        private void InitializeComponent()
        {
            this.components = new Container();
            ComponentResourceManager resources = new ComponentResourceManager(typeof(TCPServerForm));
            this.panel_left = new Panel();
            this.panel2 = new Panel();
            this.groupBox2 = new GroupBox();
            this.listViewEx1 = new ListViewEx();
            this.columnHeader_index = new ColumnHeader();
            this.columnHeader_ip = new ColumnHeader();
            this.columnHeader_port = new ColumnHeader();
            this.panel1 = new Panel();
            this.groupBox1 = new GroupBox();
            this.buttonX_stop = new ButtonX();
            this.numericUpDown1 = new NumericUpDown();
            this.checkBoxX1 = new CheckBoxX();
            this.buttonX_start = new ButtonX();
            this.labelX2 = new LabelX();
            this.bar1 = new Bar();
            this.expandableSplitter1 = new ExpandableSplitter();
            this.panel_bottom = new Panel();
            this.expandableSplitter2 = new ExpandableSplitter();
            this.panel_main = new Panel();
            this.ipdaemon1 = new Ipdaemon(this.components);
            this.receiveUc1 = new ReceiveUc();
            this.sendUc1 = new SendUc();
            this.panel_left.SuspendLayout();
            this.panel2.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.numericUpDown1.BeginInit();
            this.bar1.BeginInit();
            this.panel_bottom.SuspendLayout();
            this.panel_main.SuspendLayout();
            base.SuspendLayout();
            this.panel_left.Controls.Add(this.panel2);
            this.panel_left.Controls.Add(this.panel1);
            this.panel_left.Dock = DockStyle.Left;
            this.panel_left.Location = new Point(0, 0);
            this.panel_left.Name = "panel_left";
            this.panel_left.Size = new Size(0x101, 470);
            this.panel_left.TabIndex = 0;
            this.panel2.Controls.Add(this.groupBox2);
            this.panel2.Dock = DockStyle.Fill;
            this.panel2.Location = new Point(0, 0x61);
            this.panel2.Name = "panel2";
            this.panel2.Size = new Size(0x101, 0x175);
            this.panel2.TabIndex = 1;
            this.groupBox2.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox2.Controls.Add(this.listViewEx1);
            this.groupBox2.Location = new Point(3, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new Size(0xfb, 0x16c);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "客户端列表";
            this.listViewEx1.Border.Class = "ListViewBorder";
            this.listViewEx1.CheckBoxes = true;
            this.listViewEx1.Columns.AddRange(new ColumnHeader[] { this.columnHeader_index, this.columnHeader_ip, this.columnHeader_port });
            this.listViewEx1.Dock = DockStyle.Fill;
            this.listViewEx1.FullRowSelect = true;
            this.listViewEx1.GridLines = true;
            this.listViewEx1.Location = new Point(3, 0x11);
            this.listViewEx1.MultiSelect = false;
            this.listViewEx1.Name = "listViewEx1";
            this.listViewEx1.Size = new Size(0xf5, 0x158);
            this.listViewEx1.TabIndex = 0;
            this.listViewEx1.UseCompatibleStateImageBehavior = false;
            this.listViewEx1.View = View.Details;
            this.listViewEx1.ItemChecked += new ItemCheckedEventHandler(this.listViewEx1_ItemChecked);
            this.columnHeader_index.Text = "序号";
            this.columnHeader_index.Width = 0x37;
            this.columnHeader_ip.Text = "IP";
            this.columnHeader_ip.Width = 0x6f;
            this.columnHeader_port.Text = "端口";
            this.columnHeader_port.Width = 0x48;
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Dock = DockStyle.Top;
            this.panel1.Location = new Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new Size(0x101, 0x61);
            this.panel1.TabIndex = 0;
            this.groupBox1.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox1.Controls.Add(this.buttonX_stop);
            this.groupBox1.Controls.Add(this.numericUpDown1);
            this.groupBox1.Controls.Add(this.checkBoxX1);
            this.groupBox1.Controls.Add(this.buttonX_start);
            this.groupBox1.Controls.Add(this.labelX2);
            this.groupBox1.Location = new Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new Size(0xfb, 0x51);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "设置区";
            this.buttonX_stop.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_stop.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_stop.Enabled = false;
            this.buttonX_stop.Location = new Point(0x81, 0x34);
            this.buttonX_stop.Name = "buttonX_stop";
            this.buttonX_stop.Size = new Size(0x45, 0x17);
            this.buttonX_stop.TabIndex = 7;
            this.buttonX_stop.Text = "停止";
            this.buttonX_stop.Click += new EventHandler(this.buttonX_stop_Click);
            this.numericUpDown1.Location = new Point(0x2d, 0x10);
            int[] CS$0$0001 = new int[4];
            CS$0$0001[0] = 0x7fff;
            this.numericUpDown1.Maximum = new decimal(CS$0$0001);
            CS$0$0001 = new int[4];
            CS$0$0001[0] = 1;
            this.numericUpDown1.Minimum = new decimal(CS$0$0001);
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new Size(0x41, 0x15);
            this.numericUpDown1.TabIndex = 6;
            CS$0$0001 = new int[4];
            CS$0$0001[0] = 0x1a0a;
            this.numericUpDown1.Value = new decimal(CS$0$0001);
            this.checkBoxX1.Location = new Point(0x81, 0x10);
            this.checkBoxX1.Name = "checkBoxX1";
            this.checkBoxX1.Size = new Size(0x4b, 0x17);
            this.checkBoxX1.TabIndex = 5;
            this.checkBoxX1.Text = "自动回传";
            this.checkBoxX1.CheckedChanged += new EventHandler(this.checkBoxX1_CheckedChanged);
            this.buttonX_start.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_start.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_start.Location = new Point(0x2d, 0x34);
            this.buttonX_start.Name = "buttonX_start";
            this.buttonX_start.Size = new Size(0x45, 0x17);
            this.buttonX_start.TabIndex = 4;
            this.buttonX_start.Text = "启动";
            this.buttonX_start.Click += new EventHandler(this.buttonX_start_Click);
            this.labelX2.Location = new Point(9, 0x10);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new Size(0x29, 0x17);
            this.labelX2.TabIndex = 2;
            this.labelX2.Text = "端口:";
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = AccessibleRole.StatusBar;
            this.bar1.Dock = DockStyle.Bottom;
            this.bar1.GrabHandleStyle = eGrabHandleStyle.ResizeHandle;
            this.bar1.Location = new Point(0, 470);
            this.bar1.Name = "bar1";
            this.bar1.Size = new Size(0x341, 0x19);
            this.bar1.Stretch = true;
            this.bar1.Style = eDotNetBarStyle.Office2007;
            this.bar1.TabIndex = 0;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            this.expandableSplitter1.BackColor2 = SystemColors.ControlDarkDark;
            this.expandableSplitter1.BackColor2SchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter1.BackColorSchemePart = eColorSchemePart.PanelBackground;
            this.expandableSplitter1.ExpandFillColor = SystemColors.ControlDarkDark;
            this.expandableSplitter1.ExpandFillColorSchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter1.ExpandLineColor = SystemColors.ControlText;
            this.expandableSplitter1.ExpandLineColorSchemePart = eColorSchemePart.ItemText;
            this.expandableSplitter1.GripDarkColor = SystemColors.ControlText;
            this.expandableSplitter1.GripDarkColorSchemePart = eColorSchemePart.ItemText;
            this.expandableSplitter1.GripLightColor = Color.FromArgb(0xf4, 0xf6, 0xf8);
            this.expandableSplitter1.GripLightColorSchemePart = eColorSchemePart.BarBackground;
            this.expandableSplitter1.HotBackColor = Color.FromArgb(0xa4, 0xe7, 0xfb);
            this.expandableSplitter1.HotBackColor2 = Color.FromArgb(230, 0xf5, 0xf9);
            this.expandableSplitter1.HotBackColor2SchemePart = eColorSchemePart.ItemPressedBackground2;
            this.expandableSplitter1.HotBackColorSchemePart = eColorSchemePart.ItemPressedBackground;
            this.expandableSplitter1.HotExpandFillColor = SystemColors.ControlDarkDark;
            this.expandableSplitter1.HotExpandFillColorSchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter1.HotExpandLineColor = SystemColors.ControlText;
            this.expandableSplitter1.HotExpandLineColorSchemePart = eColorSchemePart.ItemText;
            this.expandableSplitter1.HotGripDarkColor = SystemColors.ControlDarkDark;
            this.expandableSplitter1.HotGripDarkColorSchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter1.HotGripLightColor = Color.FromArgb(0xf4, 0xf6, 0xf8);
            this.expandableSplitter1.HotGripLightColorSchemePart = eColorSchemePart.BarBackground;
            this.expandableSplitter1.Location = new Point(0x101, 0);
            this.expandableSplitter1.Name = "expandableSplitter1";
            this.expandableSplitter1.Size = new Size(3, 470);
            this.expandableSplitter1.TabIndex = 1;
            this.expandableSplitter1.TabStop = false;
            this.panel_bottom.Controls.Add(this.sendUc1);
            this.panel_bottom.Dock = DockStyle.Bottom;
            this.panel_bottom.Location = new Point(260, 0x150);
            this.panel_bottom.Name = "panel_bottom";
            this.panel_bottom.Size = new Size(0x23d, 0x86);
            this.panel_bottom.TabIndex = 2;
            this.expandableSplitter2.BackColor2 = SystemColors.ControlDarkDark;
            this.expandableSplitter2.BackColor2SchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter2.BackColorSchemePart = eColorSchemePart.PanelBackground;
            this.expandableSplitter2.Dock = DockStyle.Bottom;
            this.expandableSplitter2.ExpandFillColor = SystemColors.ControlDarkDark;
            this.expandableSplitter2.ExpandFillColorSchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter2.ExpandLineColor = SystemColors.ControlText;
            this.expandableSplitter2.ExpandLineColorSchemePart = eColorSchemePart.ItemText;
            this.expandableSplitter2.GripDarkColor = SystemColors.ControlText;
            this.expandableSplitter2.GripDarkColorSchemePart = eColorSchemePart.ItemText;
            this.expandableSplitter2.GripLightColor = Color.FromArgb(0xf4, 0xf6, 0xf8);
            this.expandableSplitter2.GripLightColorSchemePart = eColorSchemePart.BarBackground;
            this.expandableSplitter2.HotBackColor = Color.FromArgb(0xa4, 0xe7, 0xfb);
            this.expandableSplitter2.HotBackColor2 = Color.FromArgb(230, 0xf5, 0xf9);
            this.expandableSplitter2.HotBackColor2SchemePart = eColorSchemePart.ItemPressedBackground2;
            this.expandableSplitter2.HotBackColorSchemePart = eColorSchemePart.ItemPressedBackground;
            this.expandableSplitter2.HotExpandFillColor = SystemColors.ControlDarkDark;
            this.expandableSplitter2.HotExpandFillColorSchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter2.HotExpandLineColor = SystemColors.ControlText;
            this.expandableSplitter2.HotExpandLineColorSchemePart = eColorSchemePart.ItemText;
            this.expandableSplitter2.HotGripDarkColor = SystemColors.ControlDarkDark;
            this.expandableSplitter2.HotGripDarkColorSchemePart = eColorSchemePart.PanelBorder;
            this.expandableSplitter2.HotGripLightColor = Color.FromArgb(0xf4, 0xf6, 0xf8);
            this.expandableSplitter2.HotGripLightColorSchemePart = eColorSchemePart.BarBackground;
            this.expandableSplitter2.Location = new Point(260, 0x14d);
            this.expandableSplitter2.Name = "expandableSplitter2";
            this.expandableSplitter2.Size = new Size(0x23d, 3);
            this.expandableSplitter2.TabIndex = 3;
            this.expandableSplitter2.TabStop = false;
            this.panel_main.Controls.Add(this.receiveUc1);
            this.panel_main.Dock = DockStyle.Fill;
            this.panel_main.Location = new Point(260, 0);
            this.panel_main.Name = "panel_main";
            this.panel_main.Size = new Size(0x23d, 0x14d);
            this.panel_main.TabIndex = 4;
            this.ipdaemon1.About = "";
            this.ipdaemon1.InvokeThrough = null;
            this.receiveUc1.Dock = DockStyle.Fill;
            this.receiveUc1.Location = new Point(0, 0);
            this.receiveUc1.Name = "receiveUc1";
            this.receiveUc1.Size = new Size(0x23d, 0x14d);
            this.receiveUc1.TabIndex = 0;
            this.sendUc1.Dock = DockStyle.Fill;
            this.sendUc1.IP = "127.0.0.1";
            this.sendUc1.Location = new Point(0, 0);
            this.sendUc1.Name = "sendUc1";
            this.sendUc1.Port = 0x1a0a;
            this.sendUc1.Size = new Size(0x23d, 0x86);
            this.sendUc1.TabIndex = 0;
            base.AutoScaleDimensions = new SizeF(6f, 12f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.ClientSize = new Size(0x341, 0x1ef);
            base.Controls.Add(this.panel_main);
            base.Controls.Add(this.expandableSplitter2);
            base.Controls.Add(this.panel_bottom);
            base.Controls.Add(this.expandableSplitter1);
            base.Controls.Add(this.panel_left);
            base.Controls.Add(this.bar1);
            base.Icon = (Icon) resources.GetObject("$this.Icon");
            base.Name = "TCPServerForm";
            this.Text = "TCPServer";
            this.panel_left.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.numericUpDown1.EndInit();
            this.bar1.EndInit();
            this.panel_bottom.ResumeLayout(false);
            this.panel_main.ResumeLayout(false);
            base.ResumeLayout(false);
        }

        private void ipdaemon1_OnConnected(object sender, IpdaemonConnectedEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(string.Format("客户端{0}上线,{1}:{2}", e.ConnectionId, this.ipdaemon1.RemoteHost[e.ConnectionId], this.ipdaemon1.RemotePort[e.ConnectionId]), 2));
            lock (this.ObjectForLock)
            {
                if (!this.HS_Innerid_ListviewItem.ContainsKey(e.ConnectionId))
                {
                    string[] CS$0$0002 = new string[] { e.ConnectionId.ToString(), this.ipdaemon1.RemoteHost[e.ConnectionId], this.ipdaemon1.RemotePort[e.ConnectionId].ToString() };
                    ListViewItem lvi = new ListViewItem(CS$0$0002);
                    this.AddItemToListview(lvi);
                    this.HS_Innerid_ListviewItem.Add(e.ConnectionId, lvi);
                }
            }
        }

        private void ipdaemon1_OnDataIn(object sender, IpdaemonDataInEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(e.TextB, e.ConnectionId, this.ipdaemon1.RemoteHost[e.ConnectionId], false, "", this.ipdaemon1.RemotePort[e.ConnectionId], DateTime.Now, 1));
            if (this.isecho)
            {
                this.ipdaemon1.Send(e.ConnectionId, e.TextB);
            }
        }

        private void ipdaemon1_OnDisconnected(object sender, IpdaemonDisconnectedEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(string.Format("客户端{0}下线,IP:{1},Port:{2}", e.ConnectionId, this.ipdaemon1.RemoteHost[e.ConnectionId], this.ipdaemon1.RemotePort[e.ConnectionId]), 2));
            lock (this.ObjectForLock)
            {
                if (this.HS_Innerid_ListviewItem.ContainsKey(e.ConnectionId))
                {
                    this.RemoveItemToListview(this.HS_Innerid_ListviewItem[e.ConnectionId]);
                    this.HS_Innerid_ListviewItem.Remove(e.ConnectionId);
                }
            }
        }

        private void ipdaemon1_OnError(object sender, IpdaemonErrorEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(string.Format("发生错误:{0}", e.Description), 3));
        }

        private void listViewEx1_ItemChecked(object sender, ItemCheckedEventArgs e)
        {
            lock (this.ObjectForLock)
            {
                this.ClientToSendList.Clear();
                this.ClientToSendList.TrimExcess();
                foreach (ListViewItem lvi in this.listViewEx1.CheckedItems)
                {
                    this.ClientToSendList.Add(int.Parse(lvi.SubItems[0].Text));
                }
                if (this.ClientToSendChanged != null)
                {
                    this.ClientToSendChanged(this, null);
                }
            }
        }

        private void RemoveItemToListview(ListViewItem lvi)
        {
            if (this.listViewEx1.InvokeRequired)
            {
                this.listViewEx1.BeginInvoke(new AddItemToListviewCallBack(this.RemoveItemToListview), new object[] { lvi });
            }
            else
            {
                this.listViewEx1.Items.Remove(lvi);
            }
        }

        public void Send(byte[] data)
        {
            lock (this.ObjectForLock)
            {
                foreach (int clientid in this.ClientToSendList)
                {
                    try
                    {
                        this.ipdaemon1.Send(clientid, data);
                    }
                    catch (Exception ee)
                    {
                        this.receiveUc1.SetLog(new LogData(string.Format("向{1}发送数据失败:{0}", ee.Message, clientid), 3));
                    }
                }
            }
        }

        public void SetLog(LogData logdata)
        {
            this.receiveUc1.SetLog(logdata);
        }

        public bool IsStarted
        {
            get
            {
                return this.ipdaemon1.Listening;
            }
        }
    }
}

