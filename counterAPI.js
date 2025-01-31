
const cyrb53 = (str, seed = 0) => {
    let h1 = 0xdeadbeef ^ seed, h2 = 0x41c6ce57 ^ seed;
    for(let i = 0, ch; i < str.length; i++) {
        ch = str.charCodeAt(i);
        h1 = Math.imul(h1 ^ ch, 2654435761);
        h2 = Math.imul(h2 ^ ch, 1597334677);
    }
    h1  = Math.imul(h1 ^ (h1 >>> 16), 2246822507);
    h1 ^= Math.imul(h2 ^ (h2 >>> 13), 3266489909);
    h2  = Math.imul(h2 ^ (h2 >>> 16), 2246822507);
    h2 ^= Math.imul(h1 ^ (h1 >>> 13), 3266489909);
  
    return 4294967296 * (2097151 & h2) + (h1 >>> 0);
};   


const pageHit = async (name) => {
    const nameHash = cyrb53(name);
    const url = `https://api.cors.lol?url=https://api.counterapi.dev/v1/cgl_web_demos/${nameHash}/up`;
    
    const response = await fetch(url, {
        method: 'GET',
        mode: 'cors',
        headers: {
            'Origin': 'jaysmito101.github.io',
            'Content-Type': 'application/json',
            'Access-Control-Allow-Origin': '*',
        },
    });

    const data = await response.json();

    return data.count;
}

const getHitCount = async (name) => {
    const nameHash = cyrb53(name);
    const url = `https://api.cors.lol?url=https://api.counterapi.dev/v1/cgl_web_demos/${nameHash}`;

    // fix cors
    const response = await fetch(url,
        {
            mode: 'cors',
            headers: {
                'Origin': 'jaysmito101.github.io',
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
            },
        });
    const data = await response.json();

    return data.count;
}