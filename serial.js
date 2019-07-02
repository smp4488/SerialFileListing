const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');
// const PORTNAME = 'COM4';
const PORTNAME = '/dev/cu.usbmodem141101';

const fs = require('fs');
const basePath = './backing-tracks';

// const readline = require('readline');
// const rl = readline.createInterface({
//     input: process.stdin,
//     output: process.stdout
// });

const port = new SerialPort(PORTNAME, {
    baudRate: 9600,
});

const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

// Read the port data
port.on('open', () => {
    console.log('Connected');

    // port.on('data', (data) => {
    //     console.log('Data:', data);
    // });

    parser.on('data', data => {
        // Clear console
        // console.log('\033[2J');
        //process.stdout.write('\033c');
        // Show data
        console.log(data);

        // Command ":ls:{path}"
        let args = data.split(':');
        let command = args[1];
        let path = args[2];
        let file = args[3];

        if (command === 'ls'){
            console.log('Listing ' + basePath + path);
            fs.readdir(basePath + path, function (err, items) {
                let res = '<listing:' + items.join() + '>\r\n';
                console.log("Sending: ", res);
                port.write(res);
            });
        }

        if (command == 'count'){
            console.log('Count ' + basePath + path);
            fs.readdir(basePath + path, function (err, items) {
                let res = '<count:' + items.length + '>\r\n';
                console.log("Sending: ", res);
                port.write(res);
            });
        }

        if (command == 'entry') {
            console.log('Entry ' + file + " " + basePath + path);
            fs.readdir(basePath + path, function (err, items) {
                let res = '<entry:' + items[file] + '>\r\n';
                console.log("Sending: ", res);
                port.write(res);
            });
        }

        if (command == 'entryIdx') {
            console.log('Entry idx ' + file + " " + basePath + path);
            fs.readdir(basePath + path, function (err, items) {
                let res = '<entryIdx:' + items.indexOf(file) + '>\r\n';
                console.log("Sending: ", res);
                port.write(res);
            });
        }
        //console.log();
    });

    // rl.on('line', (input) => {
    //     port.write(input);
    // });
});
