namespace NetDebugTool
{
    using DevComponents.DotNetBar;
    using DevComponents.DotNetBar.Controls;
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Drawing;
    using System.Text;
    using System.Windows.Forms;

    public class ReceiveUc : UserControl
    {
        private ButtonX buttonX1;
        private ButtonX buttonX2;
        private CheckBoxX checkBoxX1;
        private IContainer components = null;
        private List<LogData> DataList;
        private GroupBox groupBox1;
        private MyRichtextBox myRichtextBox1;
        private Panel panel_bottom;
        private Panel panel_top;

        public ReceiveUc()
        {
            this.InitializeComponent();
            this.DataList = new List<LogData>();
        }

        private void AddLog(LogData log)
        {
            if (log.Log == "")
            {
                string logstr;
                this.myRichtextBox1.AppendTextAsRtf("【", this.myRichtextBox1.Font, Color.Black);
                if (this.checkBoxX1.Checked)
                {
                    this.myRichtextBox1.AppendTextAsRtf(string.Format("{0}收到数据(HEX)", log.Time.ToString("yyyy-MM-dd HH:mm:ss")), this.myRichtextBox1.Font, Color.DarkSlateGray);
                    logstr = CVT.ByteToHexStr(log.Data);
                }
                else
                {
                    this.myRichtextBox1.AppendTextAsRtf(string.Format("{0}收到数据", log.Time.ToString("yyyy-MM-dd HH:mm:ss")), this.myRichtextBox1.Font, Color.DarkSlateGray);
                    logstr = Encoding.Default.GetString(log.Data);
                }
                if (!log.IsTcpClient)
                {
                    this.myRichtextBox1.AppendTextAsRtf(string.Format(",{0}:", log.IP), this.myRichtextBox1.Font, Color.SandyBrown);
                    this.myRichtextBox1.AppendTextAsRtf(string.Format("{0}", log.Port), this.myRichtextBox1.Font, Color.RoyalBlue);
                }
                this.myRichtextBox1.AppendTextAsRtf(string.Format(" Len:{0}", log.Data.Length), this.myRichtextBox1.Font, Color.Silver);
                this.myRichtextBox1.AppendTextAsRtf("】:", this.myRichtextBox1.Font, Color.Black);
                this.myRichtextBox1.AppendTextAsRtf(string.Format("{0}\r\n", logstr), this.myRichtextBox1.Font, Color.DarkBlue);
            }
            else
            {
                Color clr = Color.Green;
                switch (log.Loglevel)
                {
                    case 1:
                        clr = Color.Green;
                        break;

                    case 2:
                        clr = Color.Blue;
                        break;

                    case 3:
                        clr = Color.Red;
                        break;

                    default:
                        clr = Color.Blue;
                        break;
                }
                this.myRichtextBox1.AppendTextAsRtf(string.Format("【{0}:{1}】\r\n", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"), log.Log), this.myRichtextBox1.Font, clr);
            }
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            this.myRichtextBox1.Clear();
            lock (this.DataList)
            {
                this.DataList.Clear();
                this.DataList.TrimExcess();
            }
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            if (this.myRichtextBox1.Text.Replace(" ", "").Length == 0)
            {
                MessageBoxEx.Show("没有数据！");
            }
            else
            {
                SaveFileDialog sf = new SaveFileDialog {
                    DefaultExt = "txt"
                };
                if (sf.ShowDialog(this) == DialogResult.OK)
                {
                    this.myRichtextBox1.SaveFile(sf.FileName, RichTextBoxStreamType.PlainText);
                }
            }
        }

        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            this.myRichtextBox1.Clear();
            lock (this.DataList)
            {
                foreach (LogData logdata in this.DataList)
                {
                    this.SetTextSafe(logdata);
                }
            }
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
            this.panel_bottom = new Panel();
            this.buttonX2 = new ButtonX();
            this.buttonX1 = new ButtonX();
            this.checkBoxX1 = new CheckBoxX();
            this.panel_top = new Panel();
            this.groupBox1 = new GroupBox();
            this.myRichtextBox1 = new MyRichtextBox();
            this.panel_bottom.SuspendLayout();
            this.panel_top.SuspendLayout();
            this.groupBox1.SuspendLayout();
            base.SuspendLayout();
            this.panel_bottom.Controls.Add(this.buttonX2);
            this.panel_bottom.Controls.Add(this.buttonX1);
            this.panel_bottom.Controls.Add(this.checkBoxX1);
            this.panel_bottom.Dock = DockStyle.Bottom;
            this.panel_bottom.Location = new Point(3, 200);
            this.panel_bottom.Name = "panel_bottom";
            this.panel_bottom.Size = new Size(0x11a, 0x27);
            this.panel_bottom.TabIndex = 0;
            this.buttonX2.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX2.Location = new Point(3, 6);
            this.buttonX2.Name = "buttonX2";
            this.buttonX2.Size = new Size(50, 0x17);
            this.buttonX2.TabIndex = 2;
            this.buttonX2.Text = "保存";
            this.buttonX2.Click += new EventHandler(this.buttonX2_Click);
            this.buttonX1.AccessibleRole = AccessibleRole.PushButton;
            this.buttonX1.Location = new Point(0x3b, 6);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new Size(0x34, 0x17);
            this.buttonX1.TabIndex = 1;
            this.buttonX1.Text = "清空";
            this.buttonX1.Click += new EventHandler(this.buttonX1_Click);
            this.checkBoxX1.Location = new Point(0x75, 6);
            this.checkBoxX1.Name = "checkBoxX1";
            this.checkBoxX1.Size = new Size(0x4b, 0x17);
            this.checkBoxX1.TabIndex = 0;
            this.checkBoxX1.Text = "Hex显示";
            this.checkBoxX1.CheckedChanged += new EventHandler(this.checkBoxX1_CheckedChanged);
            this.panel_top.Controls.Add(this.myRichtextBox1);
            this.panel_top.Dock = DockStyle.Fill;
            this.panel_top.Location = new Point(3, 0x11);
            this.panel_top.Name = "panel_top";
            this.panel_top.Size = new Size(0x11a, 0xb7);
            this.panel_top.TabIndex = 1;
            this.groupBox1.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Top;
            this.groupBox1.Controls.Add(this.panel_top);
            this.groupBox1.Controls.Add(this.panel_bottom);
            this.groupBox1.Location = new Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new Size(0x120, 0xf2);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "数据接收区";
            this.myRichtextBox1.DetectUrls = false;
            this.myRichtextBox1.Dock = DockStyle.Fill;
            this.myRichtextBox1.HiglightColor = Color.White;
            this.myRichtextBox1.Location = new Point(0, 0);
            this.myRichtextBox1.Name = "myRichtextBox1";
            this.myRichtextBox1.Size = new Size(0x11a, 0xb7);
            this.myRichtextBox1.TabIndex = 0;
            this.myRichtextBox1.Text = "";
            this.myRichtextBox1.TextColor = Color.Black;
            base.AutoScaleDimensions = new SizeF(6f, 12f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.Controls.Add(this.groupBox1);
            base.Name = "ReceiveUc";
            base.Size = new Size(0x126, 0xf8);
            this.panel_bottom.ResumeLayout(false);
            this.panel_top.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            base.ResumeLayout(false);
        }

        public void SetLog(LogData logdata)
        {
            lock (this.DataList)
            {
                this.DataList.Add(logdata);
            }
            this.SetTextSafe(logdata);
        }

        private void SetTextSafe(LogData log)
        {
            if (this.myRichtextBox1.InvokeRequired)
            {
                this.myRichtextBox1.BeginInvoke(new SetTextCallBack(this.SetTextSafe), new object[] { log });
            }
            else
            {
                this.AddLog(log);
                this.myRichtextBox1.ScrollToCaret();
            }
        }
    }
}

