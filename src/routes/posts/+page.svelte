<script lang="ts">
	import PostList from '$lib/components/PostList.svelte';
	import PostPreview from '$lib/components/PostPreview.svelte';
	import type { PageData } from './$types';

	export let data: PageData;
</script>

<article class="mx-auto max-w-3xl">
	<h1 class="text-2xl font-medium text-gray-900 dark:text-gray-50 sm:text-3xl">Posts</h1>

	<PostList posts={data.posts} />
</article>
