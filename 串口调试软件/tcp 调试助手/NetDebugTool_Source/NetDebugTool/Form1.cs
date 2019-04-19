namespace NetDebugTool
{
    using DevComponents.DotNetBar;
    using DevComponents.DotNetBar.Rendering;
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.Windows.Forms;

    public class Form1 : Office2007Form
    {
        private Bar bar1;
        private ButtonItem buttonItem_tcpc;
        private ButtonItem buttonItem_tcps;
        private ButtonItem buttonItem1;
        private IContainer components = null;
        private ContextMenuStrip contextMenuStrip1;
        private LabelItem labelItem1;
        private LabelItem labelItem2;
        private RibbonControl ribbonControl1;
        private ToolStripMenuItem 平ToolStripMenuItem;

        public Form1()
        {
            this.InitializeComponent();
            this.MyInitialize();
        }

        private void buttonItem_tcpc_Click(object sender, EventArgs e)
        {
            new TCPClinetForm { MdiParent = this }.Show();
        }

        private void buttonItem1_Click(object sender, EventArgs e)
        {
            new TCPServerForm { MdiParent = this }.Show();
        }

        private void buttonItem1_Click_1(object sender, EventArgs e)
        {
            new UDPForm { MdiParent = this }.Show();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            base.TopMost = true;
            SplashForm.CloseSplash();
            base.TopMost = true;
            base.TopMost = false;
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Name = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load_1);
            this.ResumeLayout(false);

        }

        private void MyInitialize()
        {
            base.IsMdiContainer = true;
        }

        private void Form1_Load_1(object sender, EventArgs e)
        {

        }
    }
}

