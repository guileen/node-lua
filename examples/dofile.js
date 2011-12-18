var lua = require('../build/Release/node_lua.node');

var lua_file = __dirname + '/main.lua';

var out = lua.dofile(lua_file, function(ret){
    console.log('callback:' + ret);
});
console.log('return:' + out);

var out = lua.dofileSync(lua_file);
console.log('dofile sync:' + out);

