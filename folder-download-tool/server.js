const express = require('express');
const fs = require('fs');
const path = require('path');
const archiver = require('archiver');
const { promisify } = require('util');
const stat = promisify(fs.stat);
const readdir = promisify(fs.readdir);

const app = express();
const PORT = 3000;

// 静态文件服务（让前端能访问到 index.html）
app.use(express.static(__dirname));

// 接口：获取两个文件夹的状态（是否存在及大小）
app.get('/api/status', async (req, res) => {
    const folders = ['with_compiler', 'c+_compiler'];
    const result = {};

    for (const name of folders) {
        const folderPath = path.join(__dirname, name);
        let exists = false;
        let size = 0;

        try {
            const stats = await stat(folderPath);
            if (stats.isDirectory()) {
                exists = true;
                // 计算文件夹总大小（遍历所有文件）
                const files = await readdir(folderPath, { withFileTypes: true });
                let totalSize = 0;
                for (const file of files) {
                    const filePath = path.join(folderPath, file.name);
                    const fileStat = await stat(filePath);
                    if (fileStat.isFile()) {
                        totalSize += fileStat.size;
                    }
                }
                size = totalSize;
            }
        } catch (err) {
            // 文件夹不存在，保持默认
        }

        result[name] = { exists, size };
    }

    res.json(result);
});

// 接口：下载指定文件夹（打包为 ZIP）
app.get('/download/:folder', async (req, res) => {
    const folderName = req.params.folder;
    const folderPath = path.join(__dirname, folderName);

    // 检查文件夹是否存在
    try {
        const stats = await stat(folderPath);
        if (!stats.isDirectory()) {
            return res.status(404).send('文件夹不存在');
        }
    } catch (err) {
        return res.status(404).send('文件夹不存在');
    }

    // 设置响应头，让浏览器下载 ZIP
    res.setHeader('Content-Type', 'application/zip');
    res.setHeader('Content-Disposition', `attachment; filename="${folderName}.zip"`);

    // 使用 archiver 打包
    const archive = archiver('zip', { zlib: { level: 9 } });
    archive.pipe(res);

    // 将文件夹内容添加到 ZIP（保留文件夹结构）
    archive.directory(folderPath, false);

    // 完成打包
    await archive.finalize();
});

app.listen(PORT, () => {
    console.log(`服务器运行在 http://localhost:${PORT}`);
    console.log(`请将 with_compiler 和 c+_compiler 文件夹放在与 server.js 相同的目录下`);
});