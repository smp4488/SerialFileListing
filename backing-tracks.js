const fs = require('fs');
const path = './backing-tracks/'

let getDirectoryListing = fs.readdir(path, function (err, items) {
    console.log(items.join());

    for (var i = 0; i < items.length; i++) {
        console.log(items[i]);
    }
});

module.exports = {
    getDirectoryListing
}