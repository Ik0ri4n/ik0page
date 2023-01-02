<script lang="ts">
	import type { PageData } from './$types';
	import type Post from '$lib/types/post';
	import type { SvelteComponent } from 'svelte';
	import RenderedPost from '$lib/components/RenderedPost.svelte';

	export let data: PageData;
    let {Content, meta} = data;
</script>

<RenderedPost {Content} {meta} />
