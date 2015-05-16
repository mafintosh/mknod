var bindings = require('bindings')('mknod')
var noop = function () {}

module.exports = function (path, mode, dev, cb) {
  bindings.mknod(path, mode, dev, cb || noop)
}
