namespace NetDebugTool
{
    using DevComponents.DotNetBar;
    using DevComponents.DotNetBar.Controls;
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.Text;
    using System.Windows.Forms;

    public class SendUc : UserControl
    {
        private ButtonX buttonX_send;
        private CheckBoxX checkBoxX1;
        private CheckBoxX checkBoxX2;
        private IContainer components = null;
        private GroupBox groupBox1;
        public ISendUC isend;
        private Label label1;
        private LabelX labelX1;
        private NumericUpDown numericUpDown_loc_port;
        private NumericUpDown numericUpDown1;
        private Panel panel_bottom;
        private Panel panel_main;
        private TextBoxX textBoxX_ip;
        private TextBoxX textBoxX1;
        private Timer timer1;

        public SendUc()
        {
            this.InitializeComponent();
        }

        private void buttonX_send_Click(object sender, EventArgs e)
        {
            if (!this.isend.IsStarted)
            {
                this.isend.SetLog(new LogData("尚未启动服务或未连接", 3));
            }
            else
            {
                if (!this.textBoxX_ip.Visible)
                {
                    if (this.isend.GetClientsForSend().Count == 0)
                    {
                        this.isend.SetLog(new LogData("请在先在左边列表框里选中要发送的列表", 3));
                        return;
                    }
                }
                else if (this.textBoxX_ip.Text.Length == 0)
                {
                    this.isend.SetLog(new LogData("请输入目标IP和端口号", 3));
                    return;
                }
                if (this.textBoxX1.Text.Length == 0)
                {
                    this.isend.SetLog(new LogData("请输入要发送的数据", 3));
                }
                else if (this.checkBoxX1.Checked)
                {
                    try
                    {
                        this.isend.Send(CVT.StrToHexByte(this.textBoxX1.Text));
                    }
                    catch
                    {
                        this.isend.SetLog(new LogData("非法十六进制表示方式,请确认所输入字符为(0-9)(a-z)(A-Z)", 3));
                    }
                }
                else
                {
                    this.isend.Send(Encoding.Default.GetBytes(this.textBoxX1.Text));
                }
            }
        }

        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            if (this.textBoxX1.Text.Length != 0)
            {
                if (this.checkBoxX1.Checked)
                {
                    this.textBoxX1.Text = CVT.ByteToHexStr(Encoding.Default.GetBytes(this.textBoxX1.Text));
                }
                else
                {
                    try
                    {
                        this.textBoxX1.Text = Encoding.Default.GetString(CVT.StrToHexByte(this.textBoxX1.Text));
                    }
                    catch
                    {
                        MessageBoxEx.Show("非法十六进制表示方式,请确认所输入字符为(0-9)(a-z)(A-Z)");
                    }
                }
            }
        }

        private void checkBoxX2_CheckedChanged(object sender, EventArgs e)
        {
            this.timer1.Interval = (int) this.numericUpDown1.Value;
            this.timer1.Enabled = this.checkBoxX2.Checked;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.components = new Container();
            this.groupBox1 = new GroupBox();
            this.panel_main = new Panel();
            this.textBoxX1 = new TextBoxX();
            this.panel_bottom = new Panel();
            this.checkBoxX2 = new CheckBoxX();
            this.label1 = new Label();
            this.numericUpDown1 = new NumericUpDown();
            this.buttonX_send = new ButtonX();
            this.checkBoxX1 = new CheckBoxX();
            this.timer1 = new Timer(this.components);
            this.textBoxX_ip = new TextBoxX();
            this.numericUpDown_loc_port = new NumericUpDown();
            this.labelX1 = new LabelX();
            this.groupBox1.SuspendLayout();
            this.panel_main.SuspendLayout();
            this.panel_bottom.SuspendLayout();
            this.numericUpDown1.BeginInit();
            this.numericUpDown_loc_port.BeginInit();
            base.SuspendLayout();
            this.groupBox1.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox1.Controls.Add(this.panel_main);
            this.groupBox1.Controls.Add(this.panel_bottom);
            this.groupBox1.Location = new Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new Size(0x1c0, 0xb3);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "数据发送区";
            this.panel_main.Controls.Add(this.textBoxX1);
            this.panel_main.Dock = DockStyle.Fill;
            this.panel_main.Location = new Point(3, 0x11);
            this.panel_main.Name = "panel_main";
            this.panel_main.Size = new Size(0x1ba, 0x7a);
            this.panel_main.TabIndex = 1;
            this.textBoxX1.Border.Class = "TextBoxBorder";
            this.textBoxX1.Dock = DockStyle.Fill;
            this.textBoxX1.Location = new Point(0, 0);
            this.textBoxX1.Multiline = true;
            this.textBoxX1.Name = "textBoxX1";
            this.textBoxX1.Size = new Size(0x1ba, 0x7a);
            this.textBoxX1.TabIndex = 0;
            this.textBoxX1.Text = "Test";
            this.textBoxX1.WatermarkText = "在此输入要发送的数据";
            this.textBoxX1.TextChanged += new EventHandler(this.textBoxX1_TextChanged);
            this.panel_bottom.Controls.Add(this.labelX1);
            this.panel_bottom.Controls.Add(this.numericUpDown_loc_port);
            this.panel_bottom.Controls.Add(this.textBoxX_ip);
            this.panel_bottom.Controls.Add(this.checkBoxX2);
            this.panel_bottom.Controls.Add(this.label1);
            this.panel_bottom.Controls.Add(this.numericUpDown1);
            this.panel_bottom.Controls.Add(this.buttonX_send);
            this.panel_bottom.Controls.Add(this.checkBoxX1);
            this.panel_bottom.Dock = DockStyle.Bottom;
            this.panel_bottom.Location = new Point(3, 0x8b);
            this.panel_bottom.Name = "panel_bottom";
            this.panel_bottom.Size = new Size(0x1ba, 0x25);
            this.panel_bottom.TabIndex = 0;
            this.checkBoxX2.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            this.checkBoxX2.Location = new Point(0x47, 6);
            this.checkBoxX2.Name = "checkBoxX2";
            this.checkBoxX2.Size = new Size(0x4f, 0x17);
            this.checkBoxX2.TabIndex = 7;
            this.checkBoxX2.Text = "定时发送";
            this.checkBoxX2.CheckedChanged += new EventHandler(this.checkBoxX2_CheckedChanged);
            this.label1.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            this.label1.AutoSize = true;
            this.label1.ForeColor = SystemColors.WindowText;
            this.label1.Location = new Point(0xc9, 13);
            this.label1.Name = "label1";
            this.label1.Size = new Size(0x23, 12);
            this.label1.TabIndex = 6;
            this.label1.Text = "ms/次";
            this.numericUpDown1.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            this.numericUpDown1.Location = new Point(0x97, 9);
            int[] CS$0$0000 = new int[4];
            CS$0$0000[0] = 0x4fff6;
            this.numericUpDown1.Maximum = new decimal(CS$0$0000);
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 1;
            this.numericUpDown1.Minimum = new decimal(CS$0$0000);
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new Size(0x2f, 0x15);
            this.numericUpDown1.TabIndex = 5;
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 0x3e8;
            this.numericUpDown1.Value = new decimal(CS$0$0000);
            this.buttonX_send.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX_send.Anchor = AnchorStyles.Right | AnchorStyles.Bottom;
            this.buttonX_send.Location = new Point(0x180, 6);
            this.buttonX_send.Name = "buttonX_send";
            this.buttonX_send.Size = new Size(50, 0x17);
            this.buttonX_send.TabIndex = 3;
            this.buttonX_send.Text = "发送";
            this.buttonX_send.Click += new EventHandler(this.buttonX_send_Click);
            this.checkBoxX1.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            this.checkBoxX1.Location = new Point(3, 6);
            this.checkBoxX1.Name = "checkBoxX1";
            this.checkBoxX1.Size = new Size(0x43, 0x17);
            this.checkBoxX1.TabIndex = 0;
            this.checkBoxX1.Text = "HEX发送";
            this.checkBoxX1.CheckedChanged += new EventHandler(this.checkBoxX1_CheckedChanged);
            this.timer1.Tick += new EventHandler(this.timer1_Tick);
            this.textBoxX_ip.Border.Class = "TextBoxBorder";
            this.textBoxX_ip.Location = new Point(0xf2, 7);
            this.textBoxX_ip.Name = "textBoxX_ip";
            this.textBoxX_ip.Size = new Size(0x47, 0x15);
            this.textBoxX_ip.TabIndex = 8;
            this.textBoxX_ip.Text = "127.0.0.1";
            this.textBoxX_ip.Visible = false;
            this.numericUpDown_loc_port.Location = new Point(0x144, 7);
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 0x7fff;
            this.numericUpDown_loc_port.Maximum = new decimal(CS$0$0000);
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 1;
            this.numericUpDown_loc_port.Minimum = new decimal(CS$0$0000);
            this.numericUpDown_loc_port.Name = "numericUpDown_loc_port";
            this.numericUpDown_loc_port.Size = new Size(0x31, 0x15);
            this.numericUpDown_loc_port.TabIndex = 11;
            CS$0$0000 = new int[4];
            CS$0$0000[0] = 0x1a0a;
            this.numericUpDown_loc_port.Value = new decimal(CS$0$0000);
            this.numericUpDown_loc_port.Visible = false;
            this.labelX1.Location = new Point(0x13a, 5);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new Size(7, 0x17);
            this.labelX1.TabIndex = 12;
            this.labelX1.Text = ":";
            this.labelX1.Visible = false;
            base.AutoScaleDimensions = new SizeF(6f, 12f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.Controls.Add(this.groupBox1);
            base.Name = "SendUc";
            base.Size = new Size(0x1c6, 0xb9);
            this.groupBox1.ResumeLayout(false);
            this.panel_main.ResumeLayout(false);
            this.panel_bottom.ResumeLayout(false);
            this.panel_bottom.PerformLayout();
            this.numericUpDown1.EndInit();
            this.numericUpDown_loc_port.EndInit();
            base.ResumeLayout(false);
        }

        public void ShowIPAndPortTextBox()
        {
            this.textBoxX_ip.Show();
            this.numericUpDown_loc_port.Show();
            this.labelX1.Show();
        }

        private void textBoxX1_TextChanged(object sender, EventArgs e)
        {
            if (this.checkBoxX1.Checked)
            {
                this.groupBox1.Text = string.Format("数据发送区({0}bytes)", this.textBoxX1.Text.Replace(" ", "").Length / 2);
            }
            else
            {
                this.groupBox1.Text = string.Format("数据发送区({0}bytes)", this.textBoxX1.Text.Length);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (this.isend.IsStarted)
            {
                this.buttonX_send_Click(sender, e);
            }
        }

        public string IP
        {
            get
            {
                return this.textBoxX_ip.Text;
            }
            set
            {
                this.textBoxX_ip.Text = value;
            }
        }

        public int Port
        {
            get
            {
                return (int) this.numericUpDown_loc_port.Value;
            }
            set
            {
                this.numericUpDown_loc_port.Value = value;
            }
        }
    }
}

