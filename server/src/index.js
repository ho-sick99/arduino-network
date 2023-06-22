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

router.get("/data", async (req, res) => {
    const { file } = req.query;
    const filePath = path.join(imageDirectory, file);
    const data = (await fs.readFile(filePath, 'utf8')).split(" ");
    return res.render('data', {
        time: req.query.time,
        gas: data[0],
        lox: data[1],
        text: (data[2] == 0) ? '사람이 감지되었습니다.' : '가스가 감지되었습니다.'
    });
})

router.post("/arduino", (req, res) => {
    const { gas, lox, mode } = req.body; // 가스, 레이저 센서값, 모드
    const fileName = String(Date.now()) + "_" + nanoid(); // 파일 이름 '현재 시간_랜덤문자열'
    const filePath = path.join(imageDirectory, fileName); // 저장 경로 'images'
    const fileData = gas + " " + lox + " " + mode;
    fs.writeFile(filePath, fileData, err => { // 가스, 레이저 센서값, 모드 파일에 저장
        if (err) {
            console.error('파일 저장 오류:', err);
            return res.sendStatus(500).send('서버 오류');
        }
    });
    return res.send(req.body);
})

module.exports = router;