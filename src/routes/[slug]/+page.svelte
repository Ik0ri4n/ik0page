<script lang="ts">
	import type { PageData } from './$types';
	import RenderedPost from '$lib/components/RenderedPost.svelte';

	export let data: PageData;
	let { Content, meta } = data;
</script>

<RenderedPost {Content} {meta} />
