import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import pypandoc
import os

class MarkdownToPDFConverter:
    def __init__(self, root):
        self.root = root
        self.root.title("Markdown转PDF工具 (pandoc版)")
        self.root.geometry("600x400")
        
        self.style = ttk.Style()
        self.style.theme_use('clam')
        self.create_widgets()
    
    def create_widgets(self):
        # 输入文件选择
        ttk.Label(self.root, text="Markdown文件:").grid(row=0, column=0, padx=10, pady=10, sticky="w")
        self.input_entry = ttk.Entry(self.root, width=50)
        self.input_entry.grid(row=0, column=1, padx=5, pady=10)
        ttk.Button(self.root, text="浏览...", command=self.browse_input).grid(row=0, column=2, padx=5, pady=10)
        
        # 输出文件选择
        ttk.Label(self.root, text="PDF输出路径:").grid(row=1, column=0, padx=10, pady=10, sticky="w")
        self.output_entry = ttk.Entry(self.root, width=50)
        self.output_entry.grid(row=1, column=1, padx=5, pady=10)
        ttk.Button(self.root, text="浏览...", command=self.browse_output).grid(row=1, column=2, padx=5, pady=10)
        
        # 模板文件选择 (可选)
        ttk.Label(self.root, text="模板文件 (可选):").grid(row=2, column=0, padx=10, pady=10, sticky="w")
        self.template_entry = ttk.Entry(self.root, width=50)
        self.template_entry.grid(row=2, column=1, padx=5, pady=10)
        ttk.Button(self.root, text="浏览...", command=self.browse_template).grid(row=2, column=2, padx=5, pady=10)
        
        # 转换按钮
        self.convert_btn = ttk.Button(self.root, text="转换为PDF", command=self.convert)
        self.convert_btn.grid(row=3, column=1, pady=20)
        
        # 日志区域
        self.log_text = tk.Text(self.root, height=10, state="disabled")
        self.log_text.grid(row=4, column=0, columnspan=3, padx=10, pady=10, sticky="nsew")
        
        # 配置网格布局权重
        self.root.grid_rowconfigure(4, weight=1)
        self.root.grid_columnconfigure(1, weight=1)
    
    def browse_input(self):
        filename = filedialog.askopenfilename(
            title="选择Markdown文件",
            filetypes=[("Markdown文件", "*.md"), ("所有文件", "*.*")]
        )
        if filename:
            self.input_entry.delete(0, tk.END)
            self.input_entry.insert(0, filename)
            
            # 自动设置输出文件名
            output_path = os.path.splitext(filename)[0] + ".pdf"
            self.output_entry.delete(0, tk.END)
            self.output_entry.insert(0, output_path)
    
    def browse_output(self):
        filename = filedialog.asksaveasfilename(
            title="保存PDF文件",
            defaultextension=".pdf",
            filetypes=[("PDF文件", "*.pdf"), ("所有文件", "*.*")]
        )
        if filename:
            self.output_entry.delete(0, tk.END)
            self.output_entry.insert(0, filename)
    
    def browse_template(self):
        filename = filedialog.askopenfilename(
            title="选择模板文件",
            filetypes=[("模板文件", "*.tex"), ("所有文件", "*.*")]
        )
        if filename:
            self.template_entry.delete(0, tk.END)
            self.template_entry.insert(0, filename)
    
    def log(self, message):
        self.log_text.config(state="normal")
        self.log_text.insert(tk.END, message + "\n")
        self.log_text.see(tk.END)
        self.log_text.config(state="disabled")
        self.root.update()
    
    def convert(self):
        input_file = self.input_entry.get()
        output_file = self.output_entry.get()
        template_file = self.template_entry.get() if self.template_entry.get() else None
        
        if not input_file:
            messagebox.showerror("错误", "请选择Markdown输入文件")
            return
        
        if not output_file:
            messagebox.showerror("错误", "请指定PDF输出路径")
            return
        
        try:
            self.log("开始转换...")
            self.log(f"读取文件: {input_file}")
            
            # 转换选项
            extra_args = []
            if template_file:
                self.log(f"使用模板文件: {template_file}")
                extra_args.extend(['--template', template_file])
            
            self.log("Markdown转换为PDF...")
            pypandoc.convert_file(
                input_file,
                'pdf',
                outputfile=output_file,
                extra_args=extra_args
            )
            
            self.log(f"转换完成! PDF已保存到: {output_file}")
            messagebox.showinfo("成功", "PDF转换完成!")
        
        except Exception as e:
            self.log(f"错误: {str(e)}")
            messagebox.showerror("错误", f"转换失败: {str(e)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = MarkdownToPDFConverter(root)
    root.mainloop()