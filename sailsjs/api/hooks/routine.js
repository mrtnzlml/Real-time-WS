module.exports = function WebsocketHook(sails) {
    return {
        start: function () {
            var redis = require('redis');
            var redisClient = redis.createClient();

            redisClient.on('error', function (err) {
                sails.log.error(err);
            });

            setInterval(function () {
                redisClient.smembers('devices', function (err, result) {
                    result.forEach(function (device) {
                        redisClient.hmget('device:' + device, 'last_ping', function (err, reply) {
                            var last_ping = reply[0];
                            var epoch = new Date().getTime();
                            if (epoch - last_ping < 2000) {
                                redisClient.hmset('device:' + device, 'active', true);
                            } else {
                                redisClient.hmset('device:' + device, 'active', false);
                            }
                        });
                    });
                });
            }, 1000);
            sails.log('Starting ROUTINE...');
        },
        initialize: function (cb) { // Runs automatically when the hook initializes
            var hook = this;
            hook.start();
            // You must trigger `cb` so sails can continue loading.
            // If you pass in an error, sails will fail to load, and display your error on the console.
            return cb();
        }
    }
};
