/// <reference types="@sveltejs/kit" />
declare module '*.md' {
	export { SvelteComponent as default } from 'svelte';
	export const metadata: Record<string, any>;
}

// See https://kit.svelte.dev/docs/types#app
// for information about these interfaces
// and what to do when importing types
declare namespace App {
	// interface Error {}
	// interface Locals {}
	// interface PageData {}
	// interface Platform {}
}
