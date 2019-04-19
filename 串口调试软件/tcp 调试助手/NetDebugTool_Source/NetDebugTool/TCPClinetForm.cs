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

    public class TCPClinetForm : Office2007Form, ISendUC
    {
        private ButtonX buttonX_start;
        private ButtonX buttonX_stop;
        private CheckBoxX checkBoxX1;
        private IContainer components = null;
        private GroupBox groupBox1;
        private Ipport ipport1;
        private bool isecho = false;
        private LabelX labelX1;
        private LabelX labelX2;
        private List<int> list;
        private NumericUpDown numericUpDown1;
        private Panel panel_bottom;
        private Panel panel_main;
        private Panel panel_top;
        private ReceiveUc receiveUc1;
        private SendUc sendUc1;
        private TextBoxX textBoxX_ip;

        public event EventHandler ClientToSendChanged;

        public TCPClinetForm()
        {
            this.InitializeComponent();
            this.ipport1.OnConnected += new Ipport.OnConnectedHandler(this.ipport1_OnConnected);
            this.ipport1.OnDataIn += new Ipport.OnDataInHandler(this.ipport1_OnDataIn);
            this.ipport1.OnError += new Ipport.OnErrorHandler(this.ipport1_OnError);
            this.ipport1.OnDisconnected += new Ipport.OnDisconnectedHandler(this.ipport1_OnDisconnected);
            this.sendUc1.isend = this;
            this.list = new List<int>();
            this.list.Add(1);
        }

        private void buttonX_start_Click(object sender, EventArgs e)
        {
            if (this.textBoxX_ip.Text == "")
            {
                MessageBoxEx.Show("目的IP不能为空");
            }
            else
            {
                try
                {
                    this.ipport1.Connect(this.textBoxX_ip.Text, (int) this.numericUpDown1.Value);
                    this.Text = string.Concat(new object[] { "TCP Client --- ", this.textBoxX_ip.Text, ":", (int) this.numericUpDown1.Value });
                }
                catch (Exception ee)
                {
                    this.receiveUc1.SetLog(new LogData(string.Format("启动失败:{0}", ee.Message), 3));
                }
            }
        }

        private void buttonX_stop_Click(object sender, EventArgs e)
        {
            this.buttonX_start.Enabled = true;
            this.buttonX_stop.Enabled = false;
            this.receiveUc1.SetLog(new LogData(string.Format("已断开", new object[0]), 1));
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
            return this.list;
        }

        private void InitializeComponent()
        {
            this.components = new Container();
            ComponentResourceManager resources = new ComponentResourceManager(typeof(TCPClinetForm));
            this.panel_top = new Panel();
            this.groupBox1 = new GroupBox();
            this.buttonX_stop = new ButtonX();
            this.buttonX_start = new ButtonX();
            this.checkBoxX1 = new CheckBoxX();
            this.textBoxX_ip = new TextBoxX();
            this.labelX1 = new LabelX();
            this.numericUpDown1 = new NumericUpDown();
            this.labelX2 = new LabelX();
            this.panel_bottom = new Panel();
            this.sendUc1 = new SendUc();
            this.panel_main = new Panel();
            this.receiveUc1 = new ReceiveUc();
            this.ipport1 = new Ipport(this.components);
            this.panel_top.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.numericUpDown1.BeginInit();
            this.panel_bottom.SuspendLayout();
            this.panel_main.SuspendLayout();
            base.SuspendLayout();
            this.panel_top.Controls.Add(this.groupBox1);
            this.panel_top.Dock = DockStyle.Top;
            this.panel_top.Location = new Point(0, 0);
            this.panel_top.Name = "panel_top";
            this.panel_top.Size = new Size(0x225, 0x37);
            this.panel_top.TabIndex = 0;
            this.groupBox1.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox1.Controls.Add(this.buttonX_stop);
            this.groupBox1.Controls.Add(this.buttonX_start);
            this.groupBox1.Controls.Add(this.checkBoxX1);
            this.groupBox1.Controls.Add(this.textBoxX_ip);
            this.groupBox1.Controls.Add(this.labelX1);
            this.groupBox1.Controls.Add(this.numericUpDown1);
            this.groupBox1.Controls.Add(this.labelX2);
            this.groupBox1.Location = new Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new Size(0x21f, 0x2e);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "设置区";
            this.buttonX_stop.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_stop.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            this.buttonX_stop.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_stop.Enabled = false;
            this.buttonX_stop.Location = new Point(0x1ca, 13);
            this.buttonX_stop.Name = "buttonX_stop";
            this.buttonX_stop.Size = new Size(0x45, 0x17);
            this.buttonX_stop.TabIndex = 13;
            this.buttonX_stop.Text = "断开";
            this.buttonX_stop.Click += new EventHandler(this.buttonX_stop_Click);
            this.buttonX_start.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_start.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            this.buttonX_start.ColorTable = eButtonColor.OrangeWithBackground;
            this.buttonX_start.Location = new Point(0x176, 13);
            this.buttonX_start.Name = "buttonX_start";
            this.buttonX_start.Size = new Size(0x45, 0x17);
            this.buttonX_start.TabIndex = 12;
            this.buttonX_start.Text = "连接";
            this.buttonX_start.Click += new EventHandler(this.buttonX_start_Click);
            this.checkBoxX1.Location = new Point(0x102, 11);
            this.checkBoxX1.Name = "checkBoxX1";
            this.checkBoxX1.Size = new Size(0x4b, 0x17);
            this.checkBoxX1.TabIndex = 11;
            this.checkBoxX1.Text = "自动回传";
            this.checkBoxX1.CheckedChanged += new EventHandler(this.checkBoxX1_CheckedChanged);
            this.textBoxX_ip.Border.Class = "TextBoxBorder";
            this.textBoxX_ip.Location = new Point(0x23, 13);
            this.textBoxX_ip.Name = "textBoxX_ip";
            this.textBoxX_ip.Size = new Size(100, 0x15);
            this.textBoxX_ip.TabIndex = 10;
            this.textBoxX_ip.Text = "127.0.0.1";
            this.textBoxX_ip.WatermarkText = "请输入目的IP";
            this.labelX1.Location = new Point(9, 13);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new Size(50, 0x17);
            this.labelX1.TabIndex = 9;
            this.labelX1.Text = "IP:";
            this.numericUpDown1.Location = new Point(0xb1, 13);
            int[] CS$0$0000 = new int[4];
            CS$0$0000[0] = 0x7fff;
            this.numericUpDown1.Maximum = new decimal(CS$0$0000);
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 1;
            this.numericUpDown1.Minimum = new decimal(CS$0$0000);
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new Size(0x41, 0x15);
            this.numericUpDown1.TabIndex = 8;
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 0x1a0a;
            this.numericUpDown1.Value = new decimal(CS$0$0000);
            this.labelX2.Location = new Point(0x8d, 13);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new Size(0x29, 0x17);
            this.labelX2.TabIndex = 7;
            this.labelX2.Text = "端口:";
            this.panel_bottom.Controls.Add(this.sendUc1);
            this.panel_bottom.Dock = DockStyle.Bottom;
            this.panel_bottom.Location = new Point(0, 0x115);
            this.panel_bottom.Name = "panel_bottom";
            this.panel_bottom.Size = new Size(0x225, 0x7d);
            this.panel_bottom.TabIndex = 1;
            this.sendUc1.Dock = DockStyle.Fill;
            this.sendUc1.IP = "127.0.0.1";
            this.sendUc1.Location = new Point(0, 0);
            this.sendUc1.Name = "sendUc1";
            this.sendUc1.Port = 0x1a0a;
            this.sendUc1.Size = new Size(0x225, 0x7d);
            this.sendUc1.TabIndex = 0;
            this.panel_main.Controls.Add(this.receiveUc1);
            this.panel_main.Dock = DockStyle.Fill;
            this.panel_main.Location = new Point(0, 0x37);
            this.panel_main.Name = "panel_main";
            this.panel_main.Size = new Size(0x225, 0xde);
            this.panel_main.TabIndex = 2;
            this.receiveUc1.Dock = DockStyle.Fill;
            this.receiveUc1.Location = new Point(0, 0);
            this.receiveUc1.Name = "receiveUc1";
            this.receiveUc1.Size = new Size(0x225, 0xde);
            this.receiveUc1.TabIndex = 0;
            this.ipport1.About = "";
            this.ipport1.EOL = "";
            this.ipport1.EOLB = new byte[0];
            this.ipport1.FirewallPort = 0x438;
            this.ipport1.FirewallType = IpportFirewallTypes.fwNone;
            this.ipport1.InvokeThrough = null;
            base.AutoScaleDimensions = new SizeF(6f, 12f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.ClientSize = new Size(0x225, 0x192);
            base.Controls.Add(this.panel_main);
            base.Controls.Add(this.panel_bottom);
            base.Controls.Add(this.panel_top);
            base.Icon = (Icon) resources.GetObject("$this.Icon");
            base.Name = "TCPClinetForm";
            this.Text = "TCPClinetForm";
            this.panel_top.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.numericUpDown1.EndInit();
            this.panel_bottom.ResumeLayout(false);
            this.panel_main.ResumeLayout(false);
            base.ResumeLayout(false);
        }

        private void ipport1_OnConnected(object sender, IpportConnectedEventArgs e)
        {
            if (e.StatusCode == 0)
            {
                this.state(1);
            }
            else
            {
                this.receiveUc1.SetLog(new LogData(string.Format("{0}", e.Description), 3));
            }
        }

        private void ipport1_OnDataIn(object sender, IpportDataInEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(e.TextB, -1, "", true, "", 0, DateTime.Now, 1));
            if (this.isecho)
            {
                lock (this.ipport1)
                {
                    this.ipport1.Send(e.TextB);
                }
            }
        }

        private void ipport1_OnDisconnected(object sender, IpportDisconnectedEventArgs e)
        {
            this.state(2);
        }

        private void ipport1_OnError(object sender, IpportErrorEventArgs e)
        {
            this.receiveUc1.SetLog(new LogData(string.Format("发生错误:{0}", e.Description), 3));
        }

        public void Send(byte[] data)
        {
            this.ipport1.Send(data);
        }

        public void SetLog(LogData logdata)
        {
            this.receiveUc1.SetLog(logdata);
        }

        private void state(int i)
        {
            if (this.buttonX_start.InvokeRequired)
            {
                this.buttonX_start.BeginInvoke(new StatCallback(this.state), new object[] { i });
            }
            else
            {
                switch (i)
                {
                    case 1:
                        this.buttonX_start.Enabled = false;
                        this.buttonX_stop.Enabled = true;
                        this.receiveUc1.SetLog(new LogData(string.Format("已连接", new object[0]), 1));
                        return;

                    case 2:
                        if (!this.buttonX_start.Enabled)
                        {
                            this.buttonX_start.Enabled = true;
                            this.buttonX_stop.Enabled = false;
                            this.receiveUc1.SetLog(new LogData(string.Format("已断开", new object[0]), 1));
                        }
                        return;
                }
            }
        }

        public bool IsStarted
        {
            get
            {
                return this.ipport1.Connected;
            }
        }
    }
}

