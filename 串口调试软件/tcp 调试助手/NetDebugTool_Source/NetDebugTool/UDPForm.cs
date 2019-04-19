namespace NetDebugTool
{
    using DevComponents.DotNetBar;
    using DevComponents.DotNetBar.Controls;
    using nsoftware.IPWorks;
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Drawing;
    using System.Net;
    using System.Runtime.CompilerServices;
    using System.Windows.Forms;

    public class UDPForm : Office2007Form, ISendUC
    {
        private Bar bar1;
        private ButtonX buttonX_ref_ip;
        private ButtonX buttonX_start;
        private ButtonX buttonX_stop;
        private CheckBoxX checkBoxX_feedback;
        private List<string> clients;
        private ColumnHeader columnHeader_ip;
        private ColumnHeader columnHeader_port;
        private ComboBoxEx comboBoxEx_ip;
        private IContainer components = null;
        private ExpandableSplitter expandableSplitter1;
        private ExpandableSplitter expandableSplitter2;
        private GroupBox groupBox1;
        private GroupBox groupBox2;
        private bool isecho = false;
        private LabelX labelX2;
        private LabelX labelX3;
        private ListViewEx listViewEx1;
        private NumericUpDown numericUpDown_loc_port;
        private Panel panel_bottom;
        private Panel panel_left;
        private Panel panel_main;
        private Panel panel1;
        private Panel panel2;
        private ReceiveUc receiveUc1;
        private SendUc sendUc1;
        private Udpport udpport1;

        public event EventHandler ClientToSendChanged;

        public UDPForm()
        {
            this.InitializeComponent();
            this.udpport1.OnDataIn += new Udpport.OnDataInHandler(this.udpport1_OnDataIn);
            this.udpport1.OnError += new Udpport.OnErrorHandler(this.udpport1_OnError);
            this.sendUc1.ShowIPAndPortTextBox();
            this.clients = new List<string>();
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

        private void buttonX_ref_ip_Click(object sender, EventArgs e)
        {
            this.RefreshIpList();
        }

        private void buttonX_start_Click(object sender, EventArgs e)
        {
            if (this.comboBoxEx_ip.Text == "")
            {
                MessageBoxEx.Show("本地IP不能为空");
            }
            else
            {
                try
                {
                    this.clients.Clear();
                    this.clients.TrimExcess();
                    this.udpport1.LocalHost = this.comboBoxEx_ip.Text;
                    this.udpport1.LocalPort = (int) this.numericUpDown_loc_port.Value;
                    this.udpport1.Active = true;
                    this.buttonX_start.Enabled = false;
                    this.buttonX_stop.Enabled = true;
                    this.receiveUc1.SetLog(new LogData(string.Format("启动成功,{1}:{0}", this.udpport1.LocalPort, this.udpport1.LocalHost), 1));
                    this.listViewEx1.Items.Clear();
                    this.Text = "UDP --- " + this.udpport1.LocalPort;
                }
                catch (Exception ee)
                {
                    this.receiveUc1.SetLog(new LogData(string.Format("启动失败:{0}", ee.Message), 3));
                }
            }
        }

        private void buttonX_stop_Click(object sender, EventArgs e)
        {
            try
            {
                this.udpport1.Active = false;
                this.buttonX_start.Enabled = true;
                this.buttonX_stop.Enabled = false;
                this.receiveUc1.SetLog(new LogData(string.Format("停止成功", new object[0]), 1));
            }
            catch (Exception ee)
            {
                this.receiveUc1.SetLog(new LogData(string.Format("停止失败:{0}", ee.Message), 3));
            }
        }

        private void checkBoxX_feedback_CheckedChanged(object sender, EventArgs e)
        {
            this.isecho = this.checkBoxX_feedback.Checked;
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
            return null;
        }

        private void InitializeComponent()
        {
            this.components = new Container();
            ComponentResourceManager resources = new ComponentResourceManager(typeof(UDPForm));
            this.panel_bottom = new Panel();
            this.sendUc1 = new SendUc();
            this.expandableSplitter1 = new ExpandableSplitter();
            this.checkBoxX_feedback = new CheckBoxX();
            this.buttonX_start = new ButtonX();
            this.labelX2 = new LabelX();
            this.panel_main = new Panel();
            this.receiveUc1 = new ReceiveUc();
            this.expandableSplitter2 = new ExpandableSplitter();
            this.listViewEx1 = new ListViewEx();
            this.columnHeader_ip = new ColumnHeader();
            this.columnHeader_port = new ColumnHeader();
            this.groupBox2 = new GroupBox();
            this.panel_left = new Panel();
            this.panel2 = new Panel();
            this.panel1 = new Panel();
            this.groupBox1 = new GroupBox();
            this.labelX3 = new LabelX();
            this.numericUpDown_loc_port = new NumericUpDown();
            this.buttonX_stop = new ButtonX();
            this.bar1 = new Bar();
            this.udpport1 = new Udpport(this.components);
            this.comboBoxEx_ip = new ComboBoxEx();
            this.buttonX_ref_ip = new ButtonX();
            this.panel_bottom.SuspendLayout();
            this.panel_main.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.panel_left.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.numericUpDown_loc_port.BeginInit();
            this.bar1.BeginInit();
            base.SuspendLayout();
            this.panel_bottom.Controls.Add(this.sendUc1);
            this.panel_bottom.Dock = DockStyle.Bottom;
            this.panel_bottom.Location = new Point(0xbf, 0xdf);
            this.panel_bottom.Name = "panel_bottom";
            this.panel_bottom.Size = new Size(0x1c3, 0x86);
            this.panel_bottom.TabIndex = 8;
            this.sendUc1.Dock = DockStyle.Fill;
            this.sendUc1.IP = "127.0.0.1";
            this.sendUc1.Location = new Point(0, 0);
            this.sendUc1.Name = "sendUc1";
            this.sendUc1.Port = 0x1a0a;
            this.sendUc1.Size = new Size(0x1c3, 0x86);
            this.sendUc1.TabIndex = 0;
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
            this.expandableSplitter1.Location = new Point(0xbc, 0);
            this.expandableSplitter1.Name = "expandableSplitter1";
            this.expandableSplitter1.Size = new Size(3, 0x165);
            this.expandableSplitter1.TabIndex = 7;
            this.expandableSplitter1.TabStop = false;
            this.checkBoxX_feedback.Location = new Point(2, 0x48);
            this.checkBoxX_feedback.Name = "checkBoxX_feedback";
            this.checkBoxX_feedback.Size = new Size(0x4b, 0x17);
            this.checkBoxX_feedback.TabIndex = 5;
            this.checkBoxX_feedback.Text = "自动回传";
            this.checkBoxX_feedback.CheckedChanged += new EventHandler(this.checkBoxX_feedback_CheckedChanged);
            this.buttonX_start.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_start.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_start.Location = new Point(9, 0x67);
            this.buttonX_start.Name = "buttonX_start";
            this.buttonX_start.Size = new Size(0x45, 0x17);
            this.buttonX_start.TabIndex = 4;
            this.buttonX_start.Text = "启动";
            this.buttonX_start.Click += new EventHandler(this.buttonX_start_Click);
            this.labelX2.Location = new Point(6, 0x10);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new Size(0x41, 0x17);
            this.labelX2.TabIndex = 2;
            this.labelX2.Text = "本地IP:";
            this.panel_main.Controls.Add(this.receiveUc1);
            this.panel_main.Dock = DockStyle.Fill;
            this.panel_main.Location = new Point(0xbf, 0);
            this.panel_main.Name = "panel_main";
            this.panel_main.Size = new Size(0x1c3, 0xdf);
            this.panel_main.TabIndex = 10;
            this.receiveUc1.Dock = DockStyle.Fill;
            this.receiveUc1.Location = new Point(0, 0);
            this.receiveUc1.Name = "receiveUc1";
            this.receiveUc1.Size = new Size(0x1c3, 0xdf);
            this.receiveUc1.TabIndex = 0;
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
            this.expandableSplitter2.Location = new Point(0xbc, 0x165);
            this.expandableSplitter2.Name = "expandableSplitter2";
            this.expandableSplitter2.Size = new Size(0x1c6, 3);
            this.expandableSplitter2.TabIndex = 9;
            this.expandableSplitter2.TabStop = false;
            this.listViewEx1.Border.Class = "ListViewBorder";
            this.listViewEx1.Columns.AddRange(new ColumnHeader[] { this.columnHeader_ip, this.columnHeader_port });
            this.listViewEx1.Dock = DockStyle.Fill;
            this.listViewEx1.FullRowSelect = true;
            this.listViewEx1.GridLines = true;
            this.listViewEx1.Location = new Point(3, 0x11);
            this.listViewEx1.MultiSelect = false;
            this.listViewEx1.Name = "listViewEx1";
            this.listViewEx1.Size = new Size(0xb0, 0xbd);
            this.listViewEx1.TabIndex = 0;
            this.listViewEx1.UseCompatibleStateImageBehavior = false;
            this.listViewEx1.View = View.Details;
            this.listViewEx1.SelectedIndexChanged += new EventHandler(this.listViewEx1_SelectedIndexChanged);
            this.columnHeader_ip.Text = "IP";
            this.columnHeader_ip.Width = 0x5d;
            this.columnHeader_port.Text = "端口";
            this.columnHeader_port.Width = 40;
            this.groupBox2.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox2.Controls.Add(this.listViewEx1);
            this.groupBox2.Location = new Point(3, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new Size(0xb6, 0xd1);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "客户端列表";
            this.panel_left.Controls.Add(this.panel2);
            this.panel_left.Controls.Add(this.panel1);
            this.panel_left.Dock = DockStyle.Left;
            this.panel_left.Location = new Point(0, 0);
            this.panel_left.Name = "panel_left";
            this.panel_left.Size = new Size(0xbc, 360);
            this.panel_left.TabIndex = 5;
            this.panel2.Controls.Add(this.groupBox2);
            this.panel2.Dock = DockStyle.Fill;
            this.panel2.Location = new Point(0, 0x8e);
            this.panel2.Name = "panel2";
            this.panel2.Size = new Size(0xbc, 0xda);
            this.panel2.TabIndex = 1;
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Dock = DockStyle.Top;
            this.panel1.Location = new Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new Size(0xbc, 0x8e);
            this.panel1.TabIndex = 0;
            this.groupBox1.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox1.Controls.Add(this.buttonX_ref_ip);
            this.groupBox1.Controls.Add(this.comboBoxEx_ip);
            this.groupBox1.Controls.Add(this.labelX3);
            this.groupBox1.Controls.Add(this.numericUpDown_loc_port);
            this.groupBox1.Controls.Add(this.buttonX_stop);
            this.groupBox1.Controls.Add(this.checkBoxX_feedback);
            this.groupBox1.Controls.Add(this.buttonX_start);
            this.groupBox1.Controls.Add(this.labelX2);
            this.groupBox1.Location = new Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new Size(0xb6, 0x85);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "设置区";
            this.labelX3.Location = new Point(6, 0x2b);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new Size(0x41, 0x17);
            this.labelX3.TabIndex = 11;
            this.labelX3.Text = "本地端口:";
            this.numericUpDown_loc_port.Location = new Point(0x49, 0x2d);
            int[] CS$0$0001 = new int[4];
            CS$0$0001[0] = 0x7fff;
            this.numericUpDown_loc_port.Maximum = new decimal(CS$0$0001);
            CS$0$0001 = new int[4];
            CS$0$0001[0] = 1;
            this.numericUpDown_loc_port.Minimum = new decimal(CS$0$0001);
            this.numericUpDown_loc_port.Name = "numericUpDown_loc_port";
            this.numericUpDown_loc_port.Size = new Size(0x41, 0x15);
            this.numericUpDown_loc_port.TabIndex = 10;
            CS$0$0001 = new int[4];
            CS$0$0001[0] = 0x1a0a;
            this.numericUpDown_loc_port.Value = new decimal(CS$0$0001);
            this.buttonX_stop.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_stop.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_stop.Enabled = false;
            this.buttonX_stop.Location = new Point(100, 0x67);
            this.buttonX_stop.Name = "buttonX_stop";
            this.buttonX_stop.Size = new Size(0x45, 0x17);
            this.buttonX_stop.TabIndex = 7;
            this.buttonX_stop.Text = "停止";
            this.buttonX_stop.Click += new EventHandler(this.buttonX_stop_Click);
            this.bar1.AccessibleDescription = "DotNetBar Bar (bar1)";
            this.bar1.AccessibleName = "DotNetBar Bar";
            this.bar1.AccessibleRole = AccessibleRole.StatusBar;
            this.bar1.Dock = DockStyle.Bottom;
            this.bar1.GrabHandleStyle = eGrabHandleStyle.ResizeHandle;
            this.bar1.Location = new Point(0, 360);
            this.bar1.Name = "bar1";
            this.bar1.Size = new Size(0x282, 0x19);
            this.bar1.Stretch = true;
            this.bar1.Style = eDotNetBarStyle.Office2007;
            this.bar1.TabIndex = 6;
            this.bar1.TabStop = false;
            this.bar1.Text = "bar1";
            this.udpport1.About = "";
            this.udpport1.InvokeThrough = null;
            this.udpport1.TimeToLive = 0xff;
            this.comboBoxEx_ip.DisplayMember = "Text";
            this.comboBoxEx_ip.DrawMode = DrawMode.OwnerDrawFixed;
            this.comboBoxEx_ip.FormattingEnabled = true;
            this.comboBoxEx_ip.Location = new Point(0x48, 0x11);
            this.comboBoxEx_ip.Name = "comboBoxEx_ip";
            this.comboBoxEx_ip.Size = new Size(0x66, 0x16);
            this.comboBoxEx_ip.TabIndex = 12;
            this.comboBoxEx_ip.Text = "192.168.1.66";
            this.buttonX_ref_ip.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_ref_ip.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_ref_ip.Enabled = false;
            this.buttonX_ref_ip.Location = new Point(0x49, 0x48);
            this.buttonX_ref_ip.Name = "buttonX_ref_ip";
            this.buttonX_ref_ip.Size = new Size(0x67, 0x17);
            this.buttonX_ref_ip.TabIndex = 13;
            this.buttonX_ref_ip.Text = "刷新本地IP列表";
            this.buttonX_ref_ip.Click += new EventHandler(this.buttonX_ref_ip_Click);
            base.AutoScaleDimensions = new SizeF(6f, 12f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.ClientSize = new Size(0x282, 0x181);
            base.Controls.Add(this.panel_main);
            base.Controls.Add(this.panel_bottom);
            base.Controls.Add(this.expandableSplitter1);
            base.Controls.Add(this.expandableSplitter2);
            base.Controls.Add(this.panel_left);
            base.Controls.Add(this.bar1);
            base.Icon = (Icon) resources.GetObject("$this.Icon");
            base.Name = "UDPForm";
            this.Text = "UDPForm";
            base.Load += new EventHandler(this.UDPForm_Load);
            this.panel_bottom.ResumeLayout(false);
            this.panel_main.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.panel_left.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.numericUpDown_loc_port.EndInit();
            this.bar1.EndInit();
            base.ResumeLayout(false);
        }

        private void listViewEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((this.listViewEx1.SelectedItems != null) && (this.listViewEx1.SelectedItems.Count != 0))
            {
                this.sendUc1.IP = this.listViewEx1.SelectedItems[0].SubItems[0].Text;
                this.sendUc1.Port = int.Parse(this.listViewEx1.SelectedItems[0].SubItems[1].Text);
            }
        }

        private void RefreshIpList()
        {
            try
            {
                IPAddress[] addressList = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList;
                this.comboBoxEx_ip.Items.Clear();
                if (addressList.Length == 0)
                {
                    this.comboBoxEx_ip.Items.Add("127.0.0.1");
                }
                else
                {
                    for (int i = 0; i < addressList.Length; i++)
                    {
                        this.comboBoxEx_ip.Items.Add(addressList[i].ToString());
                    }
                }
            }
            catch
            {
                this.comboBoxEx_ip.Items.Clear();
                this.comboBoxEx_ip.Items.Add("127.0.0.1");
            }
        }

        public void Send(byte[] data)
        {
            try
            {
                this.udpport1.RemoteHost = this.sendUc1.IP;
                this.udpport1.RemotePort = this.sendUc1.Port;
                this.udpport1.Send(data);
            }
            catch (Exception ee)
            {
                this.receiveUc1.SetLog(new LogData(string.Format("向{1}发送数据失败:{0}", ee.Message, this.sendUc1.IP + ":" + this.sendUc1.Port), 3));
            }
        }

        public void SetLog(LogData logdata)
        {
            this.receiveUc1.SetLog(logdata);
        }

        private void UDPForm_Load(object sender, EventArgs e)
        {
            this.RefreshIpList();
        }

        private void udpport1_OnDataIn(object sender, UdpportDataInEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(e.DatagramB, -1, e.SourceAddress, false, "", e.SourcePort, DateTime.Now, 1));
            if (this.isecho)
            {
                lock (this.udpport1)
                {
                    this.udpport1.RemoteHost = e.SourceAddress;
                    this.udpport1.RemotePort = e.SourcePort;
                    this.udpport1.Send(e.DatagramB);
                }
            }
            string id = e.SourceAddress + e.SourcePort;
            if (!this.clients.Contains(id))
            {
                this.clients.Add(id);
                ListViewItem lvi = new ListViewItem(new string[] { e.SourceAddress, e.SourcePort.ToString() });
                this.AddItemToListview(lvi);
            }
        }

        private void udpport1_OnError(object sender, UdpportErrorEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(string.Format("发生错误:{0}", e.Description), 3));
        }

        public bool IsStarted
        {
            get
            {
                return this.udpport1.Active;
            }
        }
    }
}

