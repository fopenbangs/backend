window.bangsRegistry = "https://fobend.flappygrant.com"

/**
 * Resolves a bang to a type. This is implemented in functions below.
 * @param {string} bang Bang to resolve
 * @param {string} type Type to resolve bang to
 * @returns {Promise<string>} Resolved string (based on type)
 */
async function resolve(bang, type) {
    try {
        const resp = await fetch(`${window.bangsRegistry}/resolve/${type}/${bang}`)
        return resp.text()
    } catch (error) {
        console.error(`Could not resolve bang "${bang}" to ${type}!`, error)
        return (async () => error.toString())()
    }
}

/**
 * Resolves the URL for a given bang. This uses the official FopenBangs registry.
 * @param {string} bang Bang to resolve (can optionally begin with `!`)
 * @returns {Promise<string>} Resolved URL
 */
window.bangUrl = async (bang) => {
    if (bang.startsWith("!")) bang = bang.substring(1)
    return resolve(bang, "url")
}
