const { performance } = require('perf_hooks');
const cp = require('child_process');
const fetch = require('node-fetch');
const fs = require('fs');

const fetchRetry = (...options) => new Promise(async (resolve, reject) => {
	var retries = 0, time = 1000;
	var maxRetries = 20;
	while (retries < maxRetries) {
		try {
			const response = await fetch(...options);
			return resolve(response);
		} catch (err) {
			console.log(`Error Status: ${err}`);
			console.log(`sleep ${time / 1000}s...`);
			await new Promise(resolve => setTimeout(resolve, time));
			time += 1000;
		}
		retries++;
	}
	console.log(`Too many request retries.`);
	reject();
});

(async () => {
	const token = 'OnJ6EtO7zZHIxCslMoP4BeEnaTK9j6G3T5oTQpxu5Vo=';
	const { start, step, threads, prefix } =
		await fetchRetry('https://kv.akioi.ml/api/get?' + new URLSearchParams({ token, key: 'djv-' + process.argv[2] }))
			.then(resp => resp.json())
			.then(({ value }) => JSON.parse(value))
	const miners = [];

	cp.execSync(`g++-10 miner.cpp -o miner -Ofast -funroll-loops -mavx -mavx2 -DLEN=${new TextEncoder().encode(prefix).length + 8}`);

	var startTp = performance.now(), count = 0;
	for (var i = 0; i < threads; i++)
		(id => {
			miners[id] = cp.spawn('./miner', [start + id * step, start + (id + 1) * step, prefix]);
			miners[id].result = [];
			miners[id].stdout.on('data', data => miners[id].result.push(data));
			miners[id].stdout.on('data', x => console.log(x.toString()));
			miners[id].stderr.on('data', () => {
				if (++count % threads === 0) {
					var nowTp = performance.now();
					console.log(`count: ${count * 2e6}, speed: ${(count * 2e6 * 86400 * 1000 / (nowTp - startTp) / 1e8).toFixed(3)}E/d.`);
				}
			});
		})(i);

	await Promise.all(miners.map(thread =>
		new Promise(resolve => thread.on('close', resolve)))
	);

	const result = miners.map(miner => Buffer.concat(miner.result)).join('');
	fs.writeFileSync('result', result);
	await fetchRetry('https://kv.akioi.ml/api/update', {
		method: 'POST',
		headers: { 'content-type': 'application/json' },
		body: JSON.stringify({
			key: 'djv-' + process.argv[2],
			value: JSON.stringify({
				start: start + threads * step,
				step,
				threads,
				prefix
			}),
			token
		})
	});
})();