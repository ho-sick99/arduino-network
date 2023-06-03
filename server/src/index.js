"use strict";

const fs = require("fs/promises");
const path = require('path');
const { nanoid } = require("nanoid"); // 랜덤 문자열 생성기
const router = require("express").Router();
const { imageDirectory } = require("../filePath") // 파일 저장 경로
const moment = require('moment-timezone');
const targetTimeZone = 'Asia/Seoul'; // 서울시 기준 시간

router.get("/", async (req, res) => {
    let files;
    try {
        files = await fs.readdir(imageDirectory); // 폴더 내의 파일들 읽기
    } catch (err) {
        console.error('디렉토리 읽기 오류:', err);
        res.status(500).send('서버 오류');
    }
    files = files.map(file => {
        return {
            time: moment(parseInt(file.split("_")[0])).tz(targetTimeZone).format('YYYY-MM-DD HH:mm:ss'),
            fileName: file
        }
    });

    return res.render('index', { files });
})

router.post("/arduino", (req, res) => {
    const gas = req.body.gas; // 가스 센서값
    const base64Data = req.body.image;
    const fileName = String(Date.now()) + "_" + gas + "_" + nanoid() + ".jpg"; // 파일 이름 '현재 시간_가스값_랜덤문자열_파일 확장자(jpg)'
    const filePath = path.join(imageDirectory, fileName); // 저장 경로 'images'
    fs.writeFile(filePath, base64Data, 'base64', err => { // Base64 이미지 데이터를 파일로 디코딩하여 저장
        if (err) {
            console.error('파일 저장 오류:', err);
            return res.sendStatus(500).send('서버 오류');
        }
    });
    return res.send(req.body);
})

module.exports = router;